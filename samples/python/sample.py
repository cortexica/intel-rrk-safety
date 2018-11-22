import cv2
import json
import requests

cap = cv2.VideoCapture(0)
cv2.namedWindow("Cortexica PPE sample", cv2.WND_PROP_FULLSCREEN)
cv2.setWindowProperty("Cortexica PPE sample", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)

while(True):
    ret, frame = cap.read()
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2BGRA)
    key = cv2.waitKey(1)
    
    if key & 0xFF == ord(' '):
        try:
            imencoded = cv2.imencode(".jpg", rgb)[1]
            multipart_form_data = {
                'image': ('', imencoded.tostring()),
                'options': ('', '{"detectPPE": true, "detectFaces": true, "detectPersons": true, "detectBodyParts": true, "ppeThreshold": 0.5, "faceThreshold": 0.5, "personThreshold": 0.5, "bodyPartsThreshold": 0.5}'),
            }
            response = requests.post('http://127.0.0.1:8081/ppe', files=multipart_form_data)
        except requests.exceptions.RequestException as e:  # This is the correct syntax
            print(e)
            break;
        
        json_object = json.loads(response.text)
        print('\n')

        if json_object['status'] == 50100  :
            print(json_object['errorMessage'])
        else:
            print('Inference time: ' + str(json_object['ms']) + 'ms')
            for obj in json_object['result'] or []:
                categoryClass = obj['categoryClass']
                confidence = obj['confidence']
                xStart = int(obj['xStart'] * rgb.shape[1])
                xEnd = int(obj['xEnd'] * rgb.shape[1])
                yStart = int(obj['yStart'] * rgb.shape[0])
                yEnd = int(obj['yEnd'] * rgb.shape[0])
                width = xEnd - xStart
                height = yEnd - yStart
                cv2.rectangle(rgb,(xStart, yStart),(xStart + width, yStart + height), (255, 255, 255), 2)
                cv2.putText(rgb, categoryClass,(xStart + width + 10, yStart + height), 0 , 0.5, (255, 255, 255))

                print('\n')
                print('  categoryClass: ' + categoryClass)
                print('  confidence: ' + str(confidence))
                print('  xStart: ' + str(xStart))
                print('  xEnd: ' + str(xEnd))
                print('  yStart: ' + str(yStart))
                print('  yEnd: ' + str(yEnd))
            
            cv2.imshow('Cortexica PPE sample', rgb)
            cv2.waitKey(0)
    else:
        cv2.putText(rgb, 'esc - exit', (0, 10), 1 , 1, (255, 0, 0))
        cv2.putText(rgb, 'space - single query', (0, 25), 1 , 1, (255, 0, 0))
        cv2.imshow('Cortexica PPE sample', rgb)
        if key & 0xFF == 27: # This is the escape key code
            break

cap.release()
cv2.destroyAllWindows()
