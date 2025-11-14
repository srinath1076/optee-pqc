# 🧩 OP-TEE + liboqs + PQC TA Integration (QEMU-AArch64)

> ✅ Verified through successful TA + Host build and QEMU runtime up to TEE session initialization.  
> Combines OP-TEE, liboqs (Open Quantum Safe), and a PQC Trusted Application for secure enclave testing.

---

# Clone QEMU-based OP-TEE

mkdir -p $HOME/optee
cd $HOME/optee

# Core secure OS
git clone https://github.com/OP-TEE/optee_os.git

# Normal-world client libs & supplicant
git clone https://github.com/OP-TEE/optee_client.git

# (Optional) test suite
git clone https://github.com/OP-TEE/optee_test.git

# OP-TEE–enabled Linux kernel
git clone https://github.com/OP-TEE/linux.git

# ARM Trusted Firmware-A
git clone https://github.com/ARM-software/arm-trusted-firmware.git trusted-firmware-a

## 🗂 Directory Layout

| Path | Purpose |
|------|----------|
| `$HOME/optee` | Main OP-TEE build tree (`optee_os`, `linux`, `trusted-firmware-a`, etc.) |
| `$HOME/development/liboqs` | liboqs source (Open Quantum Safe) |
| `$HOME/development/liboqs-vm/optee-pqc` | PQC integration example (Host + TA) |
| `/tmp/rootfs.ext4` | Ubuntu root filesystem image (QEMU rootfs) |
| `$HOME/development/liboqs-vm/optee-pqc/bin` | Output + shared mount between host and QEMU guest |

---

### Step 1 — Build OP-TEE Components

### 1.1 Build OP-TEE OS

```bash
cd $HOME/optee/optee_os
make -j$(nproc) \
    CROSS_COMPILE64=aarch64-linux-gnu- \
    PLATFORM=vexpress-qemu_armv8a

### 1.2 Build Trusted Firmware-A (TF-A)
cd $HOME/optee/trusted-firmware-a
make -j$(nproc) \
    CROSS_COMPILE=aarch64-linux-gnu- \
    PLAT=vexpress \
    DEBUG=1 \
    SPD=opteed

Output:
build/qemu/debug/qemu_fw.bios

### 1.3 Build OP-TEE Client Libraries
cd $HOME/optee/optee_client
make -j$(nproc) CROSS_COMPILE=aarch64-linux-gnu-

Artifacts produced:
out/export/usr/lib/libteec.so* and tee-supplicant

## 🧠 Step 2 — Build liboqs Static Library for TA

cd $HOME/development/liboqs
mkdir -p build-ta && cd build-ta

cmake ..   -DCMAKE_SYSTEM_NAME=Generic   -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc   -DCMAKE_C_FLAGS="-fPIC -D__TA__ -DNO_SYS -nostdlib -fvisibility=hidden -Os"   -DBUILD_SHARED_LIBS=OFF   -DOQS_USE_OPENSSL=OFF  -DBUILD_TESTING=OFF   -DOQS_PERMIT_UNSUPPORTED_ARCHITECTURE=ON  -DOQS_ENABLE_KEM_ALG_ml_kem_768=ON   -DOQS_ENABLE_SIG_ALG_ml_dsa_65=ON   -DOQS_ENABLE_SIG_ALG_sphincs_sha2_128f_simple=ON   -DOQS_MINIMAL_BUILD="KEM_ml_kem_768;SIG_ml_dsa_65;SIG_sphincs_sha2_128f_simple" -DOQS_EMBEDDED_BUILD=ON -DOQS_USE_CPUFEATURE_INSTRUCTIONS=OFF   -DOQS_OPT_TARGET=generic 


make -j$(nproc)

Output:
build-ta/lib/liboqs.a

cd ~/development/liboqs/build-ta

aarch64-linux-gnu-gcc -c \
  -fPIC -D__TA__ -Os \
  ../oqs_stubs.c \
  -o oqs_stubs.o

aarch64-linux-gnu-ar rcs liboqs.a ~/development/liboqs/oqs_stubs.o

## 🔐 Step 3 — Build OP-TEE PQC Example

cd $HOME/development/liboqs-vm/optee-pqc
make -j$(nproc)

Artifacts appear in bin/: pqc_host, pqc_ta.ta and pqc_ta.elf

## 🧰 Step 4 — Launch QEMU with OP-TEE Enabled

# 4.1 Boot QEMU with Ubuntu RootFS and OP-TEE Kernel

qemu-system-aarch64 \
  -nographic \
  -machine virt,secure=on,gic-version=2 \
  -cpu cortex-a57 -smp 2 -m 1024 \
  -bios $HOME/optee/trusted-firmware-a/build/qemu/debug/qemu_fw.bios \
  -kernel $HOME/optee/linux/arch/arm64/boot/Image \
  -append "console=ttyAMA0 root=/dev/vda rw rootwait" \
  -drive file=/tmp/rootfs.ext4,if=virtio,format=raw \
  -dtb $HOME/optee/linux/arch/arm64/boot/dts/arm/virt.dtb \
  -virtfs local,path=$HOME/development/liboqs-vm/optee-pqc/bin,mount_tag=host0,security_model=none,id=host0 \
  -serial mon:stdio

  ## 🧩 Step 5 — Inside QEMU (Guest Setup)

  mkdir -p /mnt/host
mount -t 9p -o trans=virtio host0 /mnt/host
ls /mnt/host
# → pqc_host, pqc_ta.ta, pqc_ta.elf

mkdir -p /lib/optee_armtz
cp /mnt/host/pqc_ta.ta /lib/optee_armtz/12345678-9abc-def0-1234-56789abcdef0.ta

cp /mnt/host/libteec.so* /usr/lib/
ldconfig

tee-supplicant -d -f /var/lib/tee

## 🚀 Step 6 — Run PQC Host Program

cd /mnt/host
./pqc_host

Expected output:

Initializing TEE context...
Opening session to PQC TA...
Keygen/Encap/Decap successful.
Session closed successfully.

