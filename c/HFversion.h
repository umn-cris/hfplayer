/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
/*
**  Edit this file to change the version of the hfplayer displayed with
**  the -v command line option
*/
//const char* HF_VERSION = "2.2"; //updated 8/21/2013:  Added warmup, setup from within hfplayer, SAS, script error handling
const char* HF_VERSION = "3.0"; //updated 04/05/2015:  Added dependency replay mode, and tons of bug fixes by Alireza

//updated 2/29/2013:  Added global error and overload handling with stop on error/overload capability
//updated 4/2/2013:   Fixed a bug in error value returned.  This version was committed to github
//updated 4/3/2013:   Code cleanup to get rid of unused code
//updated 4/4/2013:   Merged Ibra's changes into the main source
//updated 4/12/2013:   Added a performance (IOPS) print at completion (v 1.10)
//updated 4/12/2013:  changed completion queue size to match max inflight IOs
//                    resolution and cleaned up old comments.
//updated 5/8/2013, changed to nanosecond timer resolution and cleaned up old comments.
//updated 6/4/2013, merged Alireza's bunching code along with Nikhil's coding standard updates.
//updated 6/25/2013, used asytle to format and added Ibra's core counts
