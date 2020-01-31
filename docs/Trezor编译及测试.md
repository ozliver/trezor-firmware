以下操作在ubuntu-18.04.3-desktop-amd64和ubuntu 19.10 系统下测试通过
# Trezor one 编译
__环境安装__
````
apt-get update
````
__安装git、make、wget__
````
sudo apt-get -y install git make wget
````
__安装protoc__
````
sudo apt-get -y install protobuf-compiler
````
__安装python__
````
sudo apt-get -y install python3-dev python3-pip
ln -s /usr/bin/python3 /usr/bin/python  //创建链接
pip3 install pipenv
````
__安装其他依赖库__
````
sudo apt-get install libsdl2-dev libsdl2-image-dev //emulator 依赖
````
__安装交叉编译工具arm-none-eabi-gcc__
- 官网地址
<https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads>
````
wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/9-2019q4/RC2.1/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
tar xjf gcc-arm-none-eabi-9-2019-q4-major-x86\_64-linux.tar.bz2
````
__添加工具链路径，按照自己的路径修改__
````
export PATH=\"\$PATH:/<font color=#FF0000 >your-dir</font>/gcc-arm-none-eabi-9-2019-q4-major/bin/\" \>\>\~/.bashrc
source \~/.bashrc
````
__下载Trezor源码__
````
git clone https://github.com/trezor/trezor-firmware.git
cd trezor-firmware/
pipenv install
cd legacy/
make vendor //下载其他依赖库
````
__编译设备bootloader和firmware__
````
make -C vendor/libopencm3 lib/stm32/f2 //编译stm32驱动库
make
make -C bootloader/ align
make -C vendor/nanopb/generator/proto
pipenv run make -C firmware/protob
pipenv run make -C firmware
````
__编译EMULATOR__
````
cd legacy
make clean
make -C firmware/ clean
export EMULATOR=1
make
make -C emulator
make -C vendor/nanopb/generator/proto
pipenv run make -C firmware/protob
pipenv run make -C firmware
````
__运行模拟器：__
````
firmware/trezor.elf
````
# Trezor One Hardware Test
#### 1. 生成使用的bin
生成使用的bin
````
cd legacy
make clean
make -C firmware/ clean
export MEMORY_PROTECT=0 EMUlATOR=0 DEBUG_LINK=1
pipenv run ./script/cibuild
````
则可在leagcy目录下得到trezor.bin
#### 2. 升级firmware
````
cd firmware
trezorctl firmware-update -f trezor.bin
````
#### 3. 执行指令开始测试
````
cd ..
pipenv run script/test
````
# Trezor One Emulator Test
### 1. 测试准备工作
- 安装pipenv
````
pip3 install pipenv
````
或使用国内源
````
pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple
````
- 安装check
````
sudo apt-get install check
````
- 安装valgrind
````
sudo apt-get install valgrind
````
### 2. Legacy (Trezor One) 测试
- 测试case路径：./legacy/script/
- 测试内容：对legacy Emulator的测试
- 以下测试中的多条指令在trezor-firmware根目录下依次执行
````
cd legacy
make clean
make -C firmware/ clean
export EMUlATOR=1 DEBUG_LINK=1
pipenv run ./script/cibuild
pipenv run script/test
````
# Test Result
- 323 passed, 136 skipped, 1 warning in 81.86s (0:01:21)
- 仿真器/硬件测试使用同样的测试case，仿真器用时1分多钟，硬件用时约25分钟