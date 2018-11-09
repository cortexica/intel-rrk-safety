# Real-time PPE Monitoring on the Edge
powered by the latest ultra-low-power high-performance Intel Myriad X VPU

[Martin Peniak, PhD](http://www.martinpeniak.com) — [Innovation Department](https://www.cortexica.com/research-innovation/)  
martin@cortexica.com

## Abstract
Personal Protective Equipment (PPE) is the equipment that protects the user against health or safety risks. Not wearing PPE dramatically increases the chances of injuries and in many situations also of financial losses due to fines for injuries and death of workers as well as contamination caused by not wearing gloves, hairnets, shoe covers, etc. Recent advancements in edge computing hardware coupled with ever more efficient software have enabled novel solutions with the potential to prevent injuries, save lives as well as money and time. This document describes the world’s first edge compute solution for PPE ingress and real- time PPE compliance monitoring.

<p align="center">
<img src="/whitepaper/images/stats.png"/> 
</p>

## Introduction
Cortexica has been at the forefront of the machine vision revolution. We have reverse-engineered parts of the human visual cortex, which allowed us to develop a powerful image search engine nowadays widely used to solve many real-world problems. Partnering with various world-leading businesses allows us to identify numerous real-world problems that can be addressed by applying machine vision methods. Some of the solutions to these problems are often very specific while others are widely applicable and have the potential to save lives. Probably the best example of this is our family of solutions designed for PPE ingress and real-time PPE compliance monitoring (see Figures 1 and 2).

PPE is designed to protect users from serious injuries or illnesses resulting from a physical, mechanical, electrical, chemical or radiological contact. The importance of PPE is paramount because it serves as the last line of defence against an injury or death. Unfortunately, studies have demonstrated that 98% of workers said they have seen others not wearing PPE when they should have been and 30% of those said this happens regularly. Head injuries, constituting 9% of all injuries, can be fatal and yet 84% of these were caused by not wearing a helmet [1]. Most of these injuries could have been prevented had there been a system in place that would continuously monitor for PPE compliance.

Recent advancements in edge computing gave raise to novel applications designed to process data right at its source effectively minimising latency and allowing real-time processing. This paper presents a solution for PPE ingress and compliance monitoring that runs in real-time and entirely on the edge.

<figure>
<img src="/whitepaper/images/access_control.jpg"/> 
</figure>

*Fig.1 - Cortexica PPE ingress solution installed at Axis Experience Centre, Madrid. This solution detects if a person is wearing a helmet and a high visibility jacket. A person is only allowed to enter a workplace once all the PPE checks have been approved.*

<figure>
<img src="/whitepaper/images/continuous_monitoring.png"/> 
</figure>

*Fig.2 - Cortexica real-time PPE compliance monitoring solution installed at Axis Experience Centre, Madrid. This solution is continuously scanning workplace and raises an alarm if a person is not wearing a helmet and a high visibility jacket. The scanning is done using Axis pan/tilt/zoom (PTZ) camera and Axis radar capable of detecting motion within 50 meters range. The radar detects motion and then automatically moves the camera to the point of interest.*

## References
[1] U.S. Department of Labor, Bureau of Labor Statistics, Accidents Involving Head Injuries, Report 605, (Washington, D.C., Government Printing Office, July 1980) p. 1
