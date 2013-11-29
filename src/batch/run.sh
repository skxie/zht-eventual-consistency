cd eventual-consistency-zht/src
ip=$(ifconfig eth0 |grep "inet addr" |cut -d: -f2 |cut -d" " -f1)
nohup ./zhtserver -z zht.conf -n neighbor.conf
#nohup ./zhtserver -z zht.conf -n neighbor.conf -p 40000
#nohup ./zhtserver -z zht.conf -n neighbor.conf -p 40000
