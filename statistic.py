#!/usr/bin/python
# -*- coding:utf8 -*-

import linecache
import os

all_file_num = 0

ban = {
    "./venv",
    "./venv2",
    "./.vs",
    "./.vscode",
    "./cmake-build-debug"
}
line_count = 0


def print_path(level, path):
    if path in ban:
        return
    global all_file_num
    global line_count
    ''''' 
    打印一个目录下的所有文件夹和文件 
    '''
    # 所有文件夹，第一个字段是次目录的级别
    dir_list = []
    # 所有文件
    file_list = []
    # 返回一个列表，其中包含在目录条目的名称(google翻译)
    files = os.listdir(path)
    # 先添加目录级别
    dir_list.append(str(level))
    for f in files:
        if os.path.isdir(path + '/' + f):
            # 排除隐藏文件夹。因为隐藏文件夹过多
            if (f[0] == '.'):
                pass
            else:
                # 添加非隐藏文件夹
                dir_list.append(f)
        if os.path.isfile(path + '/' + f) and (".cpp" in f or ".hpp" in f or ".h" in f):
            # 添加文件
            line_count += len(linecache.getlines(path + '/' + f))
            file_list.append(f)
            # 当一个标志使用，文件夹列表第一个级别不打印
    i_dl = 0
    for dl in dir_list:
        if i_dl == 0:
            i_dl = i_dl + 1
        else:
            # 打印至控制台，不是第一个的目录
            print('-' * (int(dir_list[0])), dl)
            # 打印目录下的所有文件夹和文件，目录级别+1
            print_path((int(dir_list[0]) + 1), path + '/' + dl)
    for fl in file_list:
        # 打印文件
        print('-' * (int(dir_list[0])), fl)
        # 随便计算一下有多少个文件
        all_file_num += 1


if __name__ == '__main__':
    print_path(1, '.')
    print('总文件数 =', all_file_num)
    print('line = ', line_count)
