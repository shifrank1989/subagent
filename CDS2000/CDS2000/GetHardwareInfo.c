//
//  GetHardwareInfo.c
//  CDS2000
//
//  Created by shifrank1989 on 2018/7/31.
//  Copyright © 2018 shifrank1989. All rights reserved.
//

#include "GetHardwareInfo.h"
#include <unistd.h>
static int get_cpu_num(){
    int cpu_num;
    //获取cpu核数
    cpu_num = sysconf(_SC_NPROCESSORS_ONLN);
    return cpu_num;
}
