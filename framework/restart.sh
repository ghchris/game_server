#!/bin/bash

bin='framework'

ulimit -c unlimited

work_path='/home/caocheng/new_framework/framework'
bin_pid=`cat ${work_path}/${bin}.pid 2>/dev/null | awk -F' ' '{ print $2}'`

if test -n "${bin_pid}"; then
	`kill -9 ${bin_pid} 2>/dev/null`
fi

datetag=`date '+%m_%d_%k_%M'`

`cd ${work_path}`
#ls ./*.trace  2>/dev/null | xargs -r tar -czf "./SERVER_TRACE.${datetag}.tar.gz"

ls ./*.trace 2>/dev/null | xargs -r rm -fr

ls ./*.log.* 2>/dev/null | xargs -r rm -fr
#ls ./core.*  2>/dev/null | xargs -r tar -czf "./CORE.${datetag}.tar.gz"

ls ./core* 2>/dev/null | xargs -r rm -fr

ls ./nohup.out 2>/dev/null | xargs -r rm -fr

nohup ${work_path}/${bin} --log_dir=${work_path} --logbufsecs=0 --logbuflevel=-1 --stderrthreshold=3 --cfg=${work_path}/cfg.xml &






