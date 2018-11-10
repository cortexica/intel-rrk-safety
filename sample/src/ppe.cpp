#include <Poco/StreamCopier.h>
#include <Poco/Net/MultipartWriter.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/MultipartReader.h>
#include <Poco/URI.h>
#include <iostream>
#include <opencv2/highgui.hpp>

#include "json.h"
#include "ppe.h"

using namespace cv;
using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

// Initialise static variables for thresholds
int PPE::m_ppeThreshold       = 50;
int PPE::m_personThreshold    = 50;
int PPE::m_bodyPartsThreshold = 50;
int PPE::m_faceThreshold      = 50;

PPE::PPE() : m_windowName("Cortexica PPE sample")
{
    namedWindow(m_windowName, WINDOW_NORMAL);
    resizeWindow(m_windowName, 1920, 1080);
    setWindowProperty(m_windowName, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

    createTrackbar("PPE detection threshold", m_windowName, &m_ppeThreshold, 100, onTrackbar);
    createTrackbar("Person detection threshold", m_windowName, &m_personThreshold, 100, onTrackbar);
    createTrackbar("Body parts detectiwon threshold", m_windowName, &m_bodyPartsThreshold, 100, onTrackbar);
    createTrackbar("Face detection threshold", m_windowName, &m_faceThreshold, 100, onTrackbar);
}

int PPE::run() const
{
    VideoCapture capture(0);
    capture.set(CAP_PROP_FRAME_WIDTH, 1280);
    capture.set(CAP_PROP_FRAME_HEIGHT, 720);
    capture.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));

    if (!capture.isOpened())
    {
        cerr << "Error: Could not initialise camera" << endl;
        return EXIT_FAILURE;
    }

    bool continuousPPEInProgress = true;

    while (true)
    {
        Mat frame;
        capture.read(frame);
        putText(frame, "esc - exit", Point(0, 10), 1, 1, Scalar(255, 0, 0));
        putText(frame, "space - single query", Point(0, 25), 1, 1, Scalar(255, 0, 0));
        putText(frame, "c - continuous queries", Point(0, 40), 1, 1, Scalar(255, 0, 0));

        if (continuousPPEInProgress)
        {
            const Json::Value results = sendRequest(frame, true);
            const int         status  = results["status"].asInt();

            // The time limit for the continuous mode has been reached
            if (status == 50100)
            {
                continuousPPEInProgress = false;
                continue;
            }

            drawResults(results, frame);
        }
        else
        {
            imshow(m_windowName, frame);
        }

        const char key = static_cast<char>(waitKey(1));
        if (key == ' ' && !continuousPPEInProgress)
        {
            const Json::Value results = sendRequest(frame, false);
            drawResults(results, frame);
            waitKey(0);
        }
        else if (key == 'c')
        {
            continuousPPEInProgress = true;
        }
        else if (key == 27) // ESC
        {
            break;
        }
    }

    return EXIT_SUCCESS;
}

Json::Value PPE::sendRequest(const Mat& frame, const bool continuous) const
{
    const string       endpoint = continuous ? "/continuous-ppe" : "/ppe";
    const string       host     = "localhost";
    const Poco::UInt16 port     = 8081;
    HTTPClientSession  session(host, port);
    HTTPRequest        request(HTTPRequest::HTTP_POST, endpoint, HTTPMessage::HTTP_1_1);

    request.setKeepAlive(true);
    const string boundary = MultipartWriter::createBoundary();
    request.setContentType("multipart/form-data; boundary=" + boundary);
    request.setChunkedTransferEncoding(false);

    stringstream    body;
    MultipartWriter multipartWriter(body, boundary);

    MessageHeader messageHeader;
    const string  dispJson("form-data; name=\"options\"");
    messageHeader.set("Content-Disposition", dispJson);
    multipartWriter.nextPart(messageHeader);

    vector<uchar> imageData;
    imencode(".jpg", frame, imageData);
    stringstream dataStream;
    dataStream.write(reinterpret_cast<const char*>(imageData.data()), imageData.size());

    Json::Value jsonHeader;
    jsonHeader["detectPPE"]          = m_ppeThreshold > 0 ? true : false;
    jsonHeader["detectPersons"]      = m_personThreshold > 0 ? true : false;
    jsonHeader["detectBodyParts"]    = m_bodyPartsThreshold > 0 ? true : false;
    jsonHeader["detectFaces"]        = m_faceThreshold > 0 ? true : false;
    jsonHeader["ppeThreshold"]       = m_ppeThreshold / 100.0f;
    jsonHeader["personThreshold"]    = m_personThreshold / 100.0f;
    jsonHeader["bodyPartsThreshold"] = m_bodyPartsThreshold / 100.0f;
    jsonHeader["faceThreshold"]      = m_faceThreshold / 100.0f;

    Json::FastWriter fastwriter;
    body << fastwriter.write(jsonHeader);

    dataStream.rdbuf()->pubseekpos(0);
    if (dataStream.rdbuf()->in_avail() > 0)
    {
        messageHeader.clear();
        const string dispData("form-data; name=\"image\"");
        messageHeader.set("Content-Disposition", dispData);
        multipartWriter.nextPart(messageHeader);
        body << dataStream.rdbuf();
    }

    multipartWriter.close();

    const unsigned int contentLength = body.str().size();
    request.setContentLength(contentLength);

    ostream& requestStream = session.sendRequest(request); // Send request header
    requestStream << body.rdbuf();                         // Send request body
    requestStream.flush();

    Poco::Net::HTTPResponse res;
    stringstream            resultsStream;
    std::istream&           is = session.receiveResponse(res);
    Json::Reader            reader;
    Json::Value             results;
    Poco::StreamCopier::copyStream(is, resultsStream);

    if (!reader.parse(resultsStream.str(), results))
    {
        cerr << reader.getFormattedErrorMessages();
    }
    else
    {
        cout << "Received results: " << results.toStyledString() << endl;
    }

    return results;
}

void PPE::drawResults(const Json::Value& results, Mat& frame) const
{
    vector<Rect>      ppe;
    vector<Rect>      persons[2];
    vector<Rect>      objects;
    vector<string>    categoryClasses;
    vector<float>     confidenceLevels;
    const Json::Value classifications = results["result"];
    bool              helmetFound     = false;
    bool              vestFound       = false;

    for (size_t i = 0; i < classifications.size(); i++)
    {
        const Json::Value  values        = classifications.get(i, Json::Value());
        const string       categoryClass = values.get("categoryClass", Json::Value()).asString();
        const float        confidence    = values.get("confidence", Json::Value()).asFloat();
        const float        xStart        = values.get("xStart", Json::Value()).asFloat();
        const float        xEnd          = values.get("xEnd", Json::Value()).asFloat();
        const float        yStart        = values.get("yStart", Json::Value()).asFloat();
        const float        yEnd          = values.get("yEnd", Json::Value()).asFloat();
        const unsigned int width         = static_cast<unsigned int>(frame.cols);
        const unsigned int height        = static_cast<unsigned int>(frame.rows);

        categoryClasses.push_back(categoryClass);
        confidenceLevels.push_back(confidence);
        const Rect rect(Point(xStart * width, yStart * height), Point(xEnd * width, yEnd * height));

        if (categoryClass == "helmet")
        {
            helmetFound = true;
            ppe.push_back(rect);
        }
        else if (categoryClass == "vest")
        {
            vestFound = true;
            ppe.push_back(rect);
        }
        else if (categoryClass == "face")
        {
            Rect expandedRect = rect;
            expandedRect += Size(0, frame.rows - rect.height);
            expandedRect -= Point(0, rect.y);
            persons[0].push_back(rect);
            persons[1].push_back(expandedRect);
        }

        objects.push_back(rect);
    }

    const bool         ppeFound           = helmetFound && vestFound;
    const double       fontScale          = 1.15;
    const int          verticalTextOffset = 8;
    const unsigned int thickness          = 1;
    const unsigned int fontFace           = 1;

    // Draw all boxes in white
    for (size_t i = 0; i < objects.size(); i++)
    {
        rectangle(frame, objects.at(i), Scalar::all(255), thickness, cv::LINE_AA);
        Point textPosition(objects.at(i).x, objects.at(i).y - verticalTextOffset);
        putText(frame, to_string(confidenceLevels.at(i)), textPosition, fontFace, fontScale, Scalar::all(255));
        textPosition.y -= verticalTextOffset * 2;
        putText(frame, categoryClasses.at(i), textPosition, fontFace, fontScale, Scalar::all(255));
    }

    // Draw PPE and person boxes in red or green depending on whether required PPE was found or not
    for (size_t i = 0; i < persons[1].size(); i++)
    {
        Scalar colour = Scalar(0, 0, 255);
        for (size_t j = 0; j < ppe.size(); j++)
        {
            const bool intersects = ((persons[1].at(i) & ppe.at(j)).area() > 0);
            if (intersects)
            {
                colour = ppeFound ? Scalar(0, 255, 0) : Scalar(0, 0, 255);
                rectangle(frame, ppe.at(j), colour, thickness + 2, cv::LINE_AA);
                ppe.erase(ppe.begin(), ppe.begin() + j);
            }
        }
        rectangle(frame, persons[0].at(i), colour, thickness + 2, cv::LINE_AA);
    }

    imshow(m_windowName, frame);
}

void PPE::onTrackbar(int, void*)
{
}
