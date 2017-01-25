#!/bin/sh

#echo " System info "
#echo "======================================================"
#cat /proc/cpuinfo | grep "processor|name|physical id|siblings|core id"
#echo


# check input parameter
if [ "$#" -ne "1" ] ; then
        echo "input sd device name. e.g. sda "
        exit 100
fi
SD=$1


echo "======================================================="
echo "CPU Architecture "
lscpu

echo "======================================================="
echo "Current Scheduler" 
cat /sys/block/$SD/queue/scheduler
echo "======================================================="
echo "Current Merging Algorithms : "
cat /sys/block/$SD/queue/nomerges

 if [ "$(cat /sys/block/$SD/queue/nomerges)" = 2   ]; then
     echo All Merging Algorithms Disabled
	 elif [ "$( cat /sys/block/$SD/queue/nomerges)" != 2  ]; then
	 echo Merging Algorithms Enabled	
 fi

echo "======================================================="
echo "Number of Physical cores per cpu "
grep "cpu cores" /proc/cpuinfo | sort -u | cut -d ":" -f2
echo "======================================================="
echo "Number of Physical Processors : "
grep "physical id" /proc/cpuinfo | sort -u | wc -l
echo "======================================================="
echo "Number of Logical cores : " 
grep -c "processor" /proc/cpuinfo
echo "======================================================="
echo "Number of Siblings : " 
grep "siblings" /proc/cpuinfo | sort -u | cut -d ":" -f2

 if [ "$(grep "siblings" /proc/cpuinfo | sort -u | cut -d ":" -f2)" = "$( grep "cpu cores" /proc/cpuinfo | sort -u | cut -d ":" -f2)" ]; then
   echo Hyperthread Disabled
  elif [ "$(grep "siblings" /proc/cpuinfo | sort -u | cut -d ":" -f2)" != "$( grep "cpu cores" /proc/cpuinfo | sort -u | cut -d ":" -f2)" ]; then
   echo Hyperthread Enabled
 fi

 #read -p " Would you like to use the correct system config (yes/no)? " RESP

 #if [ "$RESP" = "yes" ]; then

echo "Disabling Merging Algorithms... "
echo 2 > /sys/block/$SD/queue/nomerges
echo "Disabling Re-ordering of commands... "
echo noop > /sys/block/$SD/queue/scheduler

echo 2048 > /sys/block/$SD/queue/nr_requests
echo 2048 > /sys/block/$SD/device/queue_depth


echo "Disabling Hyperthreading.."
 CPUS_TO_SKIP=" $(cat /sys/devices/system/cpu/cpu*/topology/thread_siblings_list | cut -d '-' -f 1 | sort | uniq | tr "\r\n" "  ") "
  for CPU_PATH in /sys/devices/system/cpu/cpu[0-9]*; do
  CPU="$(echo $CPU_PATH | tr -cd "0-9")"
 echo "$CPUS_TO_SKIP" | grep " $CPU" > /dev/null
   if [ $? -ne 0 ]; then
echo 0 > $CPU_PATH/online
   
   fi
  done
echo "set IRQ affinity for 80-81 to core 0"
echo 1 > /proc/irq/81/smp_affinity
echo 1 > /proc/irq/80/smp_affinity
#elif [ "$RESP" != "yes"  ]; then
#exit;
#fi

echo 0 > /proc/sys/kernel/randomize_va_space
