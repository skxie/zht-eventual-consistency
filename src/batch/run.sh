export USER_LIB=/mnt/common/datasys2/install/lib/
export USER_INCLUDE=/mnt/common/datasys2/install/include/
export LD_LIBRARY_PATH=/mnt/common/datasys2/install/lib/
cd zht-eventual-consistency/src
ip=$(ifconfig eth0 |grep "inet addr" |cut -d: -f2 |cut -d" " -f1)
nohup ./zhtserver -z zht.conf -n neighbor.conf &
nohup ./zhtserver -z zht.conf -n neighbor.conf -p 50002 &
nohup ./zhtserver -z zht.conf -n neighbor.conf -p 50004 &
