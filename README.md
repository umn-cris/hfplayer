hfreplay 3.0
========

High Fidelity Workload Replay Engine

Authors: Alireza Haghdoost, Jerry Fredin, Weiping He, Ibra Fall, Sai Susarla, Bingzhe Li

Center for Research in Intelligent Storage (CRIS)
University of Minnesota
http://cris.cs.umn.edu

About
========

The goal of this study is to develop a scalable, timing accurate, replay engine that can faithfully preserve important characteristics of the original block workload trace. Developing a high fidelity replay tool is quite challenging for high performance storage systems. Because actual workload that hit the storage system is a mixed IO operation of multiple hosts which might have millions of IO operation per second and thousands of IO operation in-flight at a certain time. Therefore, faithfully replay a workload from as few as possible hosts is quite difficult which requires well understanding of IO subsystem operations and latency.  Although it is prevalent to replay traces at the level which the traces were captured, replay the trace at the controller level is not feasible because of the hardware limitation of storage system controller. On the other hand, replay the trace from the kernel space is not appropriate because of the portability issue. Therefore, we have developed a tool to replay a workload trace from user space and monitor kernel IO stack to maintain the replay fidelitys. 

This project has been sponsered by Center for Research in Intelligent Storage (CRIS) and made open-source by CRIS member committee. 


Setup
========
Setup involves regural Linux make procedure. libaio-dev and libboost-graph-dev library packages is required for builing the tool in addition to regular build essentials. 


Run
========
A sample run shell script is provided in bin directory. run hfplayer and depAnalyser to get more usage details. 
Running hfplayer:
1. Check disk information: sudo fdisk -l
2. Find an applicable disk partition, e.g. /dev/sda8. Note that your disk partition should not have data since hfplayer might overwrite AND distroy filesystem on your partition. 
3. Change the sampleConf-sda8.cvs file in bin/ using the selected partition 
4. Change to suitable lun numbers in the sampleConf-sda8.cvs 
4. [optional] Change the limitations of request sizes, timestamps and offset in the configuration file "sampleConf-sda8.cvs "
5. Go to bin/, type "./run.sh" to run the tool.



Support
=======
Please post your question in the github Issues page. 
https://github.com/umn-cris/hfplayer/issues


Citation
=========
This work is presented first in the 15th USENIX Conference on File and Storage Technologies (FAST'17). 
https://www.usenix.org/conference/fast17/technical-sessions/presentation/haghdoost

License
=======
© Regents of the University of Minnesota. This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.


