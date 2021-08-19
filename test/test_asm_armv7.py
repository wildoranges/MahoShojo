#!/usr/bin/env python3
import subprocess
import os

ASMBuild_ptn = '"{}" "-S" "-o" "{}" "{}"'
ExeGen_ptn = '"gcc" "-o" "{}" "{}" "../lib/libsysy.a"'
Exe_ptn = '"{}"'

def eval(EXE_PATH, TEST_BASE_PATH, timeout):
    print('===========TEST START===========')
    print('now in {}'.format(TEST_BASE_PATH))
    succ = True
    for case in testcases:
        print('Case %s:' % case, end='')
        TEST_PATH = TEST_BASE_PATH + case
        SY_PATH = TEST_BASE_PATH + case + '.sy'
        ASM_PATH = TEST_BASE_PATH + case + '.s'
        INPUT_PATH = TEST_BASE_PATH + case + '.in'
        OUTPUT_PATH = TEST_BASE_PATH + case + '.out'
        need_input = testcases[case]

        ASMBuild_result = subprocess.run(ASMBuild_ptn.format(EXE_PATH, ASM_PATH, SY_PATH), shell=True, stderr=subprocess.PIPE)
        if ASMBuild_result.returncode == 0:
            input_option = None
            if need_input:
                with open(INPUT_PATH, "rb") as fin:
                    input_option = fin.read()

            try:
                subprocess.run(ExeGen_ptn.format(TEST_PATH, ASM_PATH), shell=True, stderr=subprocess.PIPE, timeout=timeout)
                result = subprocess.run(Exe_ptn.format(TEST_PATH), shell=True, input=input_option, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=timeout)
                out = result.stdout.split(b'\n')
                if result.returncode != b'':
                    out.append(str(result.returncode).encode())
                for i in range(len(out)-1, -1, -1):
                    if out[i] == b'':
                        out.remove(b'')
                with open(OUTPUT_PATH, "rb") as fout:
                    i = 0
                    Success_flag = True
                    for line in fout.readlines():
                        line = line.strip(b'\n')
                        if line == '':
                            continue
                        if out[i].strip(b'\r') != line:
                            Success_flag = False
                            succ = False
                            print(result.stdout[:100], result.returncode, out[i][:100], line[:100], end='')
                            print('\t\033[31mWrong Answer\033[0m')
                        i = i + 1
                    #if Success_flag == True:
                    #    print('\t\033[32mSuccess\033[0m')
                print(result.stderr.decode())
            except Exception as _:
                succ = False
                print(_, end='')
                print('\t\033[31mCodeGen or CodeExecute Fail\033[0m')
            finally:
                subprocess.call(["rm", "-rf", TEST_PATH, TEST_PATH])
                subprocess.call(["rm", "-rf", TEST_PATH, TEST_PATH + ".s"])
                subprocess.call(["rm", "-rf", TEST_PATH, TEST_PATH + ".o"])
                subprocess.call(["rm", "-rf", TEST_PATH, TEST_PATH + ".ll"])

        else:
            succ = False
            print(ASMBuild_result.stderr)
            print('\t\033[31mASMBuilder Fail\033[0m')
    if succ:
        print('\t\033[32mSuccess\033[0m in dir {}'.format(TEST_BASE_PATH))
    else:
        print('\t\033[31mFail\033[0m in dir {}'.format(TEST_BASE_PATH))

    print('============TEST END============')


if __name__ == "__main__":
    # you should only revise this
    TEST_DIRS = ['./function_test2020/',
                 './function_test2021/',
                 #'./performance_test2021_pre/'
                 ]
    #TEST_BASE_PATH = './performance_test2021_pre/'
    timeout = 50             # generally less than 50s
    # you should only revise this
    for TEST_BASE_PATH in TEST_DIRS:
        testcases = {}  # { name: need_input }
        EXE_PATH = os.path.abspath('../build/compiler')
        testcase_list = list(map(lambda x: x.split('.'), os.listdir(TEST_BASE_PATH)))
        testcase_list.sort()
        for i in range(len(testcase_list)-1, -1, -1):
            if len(testcase_list[i]) == 1:
                testcase_list.remove(testcase_list[i])
        for i in range(len(testcase_list)):
            testcases[testcase_list[i][0]] = False
        for i in range(len(testcase_list)):
            testcases[testcase_list[i][0]] = testcases[testcase_list[i][0]] | (testcase_list[i][1] == 'in')
        eval(EXE_PATH, TEST_BASE_PATH, timeout=timeout)
