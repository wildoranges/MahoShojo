#!/usr/bin/env python3
import subprocess
import os
import json
import re

IRBuild_ptn_opt = '"{}" "-nocheck" "-emit-mir" "-o" "{}" "{}"'
IRBuild_ptn_no_opt = '"{}" "-nocheck" "-O0" "-emit-mir" "-o" "{}" "{}"'
ExeGen_ptn = '"clang" "{}" "-o" "{}" "{}" "../lib/lib.c"'
Exe_ptn = '"{}"'


def eval(EXE_PATH, TEST_BASE_PATH, timeout, optimization, NUM_OF_ITER=10, opt=True):
    file_name = 'performance_test_with_opt_{}.json'.format(str(opt))
    if opt:
        IRBuild_ptn = IRBuild_ptn_opt
    else:
        IRBuild_ptn = IRBuild_ptn_no_opt
    time_ptn = re.compile(r'\d+')
    print('===========TEST START WITH OPT={}==========='.format(str(opt)))
    print('now in {}'.format(TEST_BASE_PATH))
    res = {}
    res['NUM_OF_ITER'] = NUM_OF_ITER
    succ = True
    for case in testcases:
        print('Case %s:' % case, end='')
        TEST_PATH = TEST_BASE_PATH + case
        SY_PATH = TEST_BASE_PATH + case + '.sy'
        LL_PATH = TEST_BASE_PATH + case + '.ll'
        INPUT_PATH = TEST_BASE_PATH + case + '.in'
        OUTPUT_PATH = TEST_BASE_PATH + case + '.out'
        need_input = testcases[case]

        IRBuild_result = subprocess.run(IRBuild_ptn.format(EXE_PATH, LL_PATH, SY_PATH), shell=True,
                                        stderr=subprocess.PIPE)
        if IRBuild_result.returncode == 0:
            input_option = None
            if need_input:
                with open(INPUT_PATH, "rb") as fin:
                    input_option = fin.read()

            try:
                total_time = 0.0
                for _ in range(NUM_OF_ITER):
                    subprocess.run(ExeGen_ptn.format(optimization, TEST_PATH, LL_PATH), shell=True,
                                   stderr=subprocess.PIPE)
                    result = subprocess.run(Exe_ptn.format(TEST_PATH), shell=True, input=input_option,
                                            stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=timeout)
                    out = result.stdout.split(b'\n')
                    if result.returncode != b'':
                        out.append(str(result.returncode).encode())
                    for i in range(len(out) - 1, -1, -1):
                        if out[i] == b'':
                            out.remove(b'')
                    with open(OUTPUT_PATH, "rb") as fout:
                        i = 0
                        Success_flag = True
                        for line in fout.readlines():
                            line = line.strip(b'\n')
                            if line == '':
                                continue
                            if out[i] != line:
                                Success_flag = False
                                succ = False
                                print(result.stdout[:100], result.returncode, out[i][:100], line[:100], end='')
                                print('\t\033[31mWrong Answer\033[0m')
                            i = i + 1
                        # if Success_flag == True:
                        #    print('\t\033[32mSuccess\033[0m')
                    out_time = result.stderr.decode()
                    time_result = time_ptn.findall(out_time)
                    total_time += float(time_result[0]) * 3600.0
                    total_time += float(time_result[1]) * 60.0
                    total_time += float(time_result[2]) * 1.0
                    total_time += float(time_result[3]) * 1e-3
                    total_time += float(time_result[4]) * 1e-6
                    # print(result.stderr.decode())
                total_time /= NUM_OF_ITER
                res[case] = total_time
                print('\t\033[32mavg time : {} s\033[0m'.format(total_time))


            except Exception as _:
                succ = False
                print(_, end='')
                print('\t\033[31mCodeGen or CodeExecute Fail\033[0m')
            finally:
                subprocess.call(["rm", "-rf", TEST_PATH, TEST_PATH])
                subprocess.call(["rm", "-rf", TEST_PATH, TEST_PATH + ".o"])
                subprocess.call(["rm", "-rf", TEST_PATH, TEST_PATH + ".ll"])

        else:
            succ = False
            print(IRBuild_result.stderr)
            print('\t\033[31mIRBuilder Fail\033[0m')
    if succ:
        print('\t\033[32mSuccess\033[0m in dir {}'.format(TEST_BASE_PATH))
    else:
        print('\t\033[31mFail\033[0m in dir {}'.format(TEST_BASE_PATH))

    f = open(file_name, 'w+')
    json.dump(res, f, indent=4)
    f.close()
    print('============TEST END============')


if __name__ == "__main__":
    # you should only revise this
    TEST_DIRS = [
        './performance_test2021_pre/'
    ]
    # TEST_BASE_PATH = './performance_test2021_pre/'
    timeout = 50  # generally less than 50s
    optimization = "-O0"  # -O0 -O1 -O2 -O3 -O4(currently = -O3) -Ofast
    # you should only revise this
    for TEST_BASE_PATH in TEST_DIRS:
        testcases = {}  # { name: need_input }
        EXE_PATH = os.path.abspath('../build/compiler')
        testcase_list = list(map(lambda x: x.split('.'), os.listdir(TEST_BASE_PATH)))
        testcase_list.sort()
        for i in range(len(testcase_list) - 1, -1, -1):
            if len(testcase_list[i]) == 1:
                testcase_list.remove(testcase_list[i])
        for i in range(len(testcase_list)):
            testcases[testcase_list[i][0]] = False
        for i in range(len(testcase_list)):
            testcases[testcase_list[i][0]] = testcases[testcase_list[i][0]] | (testcase_list[i][1] == 'in')
        eval(EXE_PATH, TEST_BASE_PATH, timeout=timeout, optimization=optimization,NUM_OF_ITER=10,opt=True)
        eval(EXE_PATH, TEST_BASE_PATH, timeout=timeout, optimization=optimization,NUM_OF_ITER=10,opt=False)
