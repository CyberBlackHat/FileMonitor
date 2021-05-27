The application monitors directory changes. 
The application is executed with command line arguments of the directories that should be monitored (space separated) 
For each directory we create file monitor listener that in turn creates dedicated thread to monitor the changes. 
All along the way I tried to make generic design as possible. Currently, the code is supporting only windows due to 
my environmental problems I could't code it for MAC or linux. Nevertheless, I provided skeleton that provide 
the ability to plug in MAC/linux implementations easily. 
The Application uses dedicated thread safe singleton Logger that is responsible for writing the log messages to the file (created with time stamp at the current working directory) 
At first design I thought to do it with dedicated thread and prepared and implemented concurrent queue to log it async, but after second thought I decided that this will be
wrong design. 

Each directory monitoring is handles by platform customize file monitoring listener (objects that inherits from FileMonitorListener) . Each such object spawns new monitoring thread that should 
monitor the changes until stop event is raised. Currently, the implementation covers only Windows OS platform. 
All the file monitoring listeners are handled in dedicated platform file monitor object (inherits from FileMonitor) that holds all required information about the 
file monitor listeners and controls them.
Each platform should define MonitorStruct that aim to handle all the directory monitoring information that help the object to handle the monitoring. 
The Windows OS File monitor listener is registering for changes events through ReadDirectoryChangesW(using dedicated callback function FileCompletionRoutine)  API 
and waits for change events or stop events using MsgWaitForMultipleObjectsEx (all the detailed implementation is at FileMonitorListenrImpWin.cpp. 
As required I've added the file MD5 hash (implementation of the MD5 is at MD5.cpp). 
I've also added the entropy calculator module( that is heuristics to measure if the file is encrypted see: https://en.wiktionary.org/wiki/Shannon_entropy ) .

Thanks,
Mordechai Yaakobi