#!/bin/bash
set -e

BIN_DIR="$HOMEDEV/development/liboqs-vm/optee-pqc/bin"
OPTEE_CLIENT="$HOME/optee/optee_client/out/export/usr"
OPTEE_TEST="$HOME/optee/optee_test/out"
OPTEE_TA_DIR="$OPTEE_TEST/ta"

echo ">>> Preparing OP-TEE runtime environment in: $BIN_DIR"
mkdir -p "$BIN_DIR"
mkdir -p "$BIN_DIR/xtest_ta"

# 1️⃣ Copy all OP-TEE client libraries (.so and .a)
echo ">>> Copying OP-TEE client libraries..."
cp "$OPTEE_CLIENT/lib/"lib*.so* "$BIN_DIR/"
cp "$OPTEE_CLIENT/lib/"lib*.a "$BIN_DIR/"

# 2️⃣ Copy tee-supplicant
echo ">>> Copying tee-supplicant..."
cp "$OPTEE_CLIENT/sbin/tee-supplicant" "$BIN_DIR/"
chmod +x "$BIN_DIR/tee-supplicant"

# 3️⃣ Copy xtest binary and all its TAs
echo ">>> Copying xtest and its Trusted Applications..."
cp "$OPTEE_TEST/xtest/xtest" "$BIN_DIR/"
find "$OPTEE_TA_DIR" -name "*.ta" -exec cp {} "$BIN_DIR/xtest_ta/" \;

# 4️⃣ Verify PQC binaries already exist
echo ">>> Verifying PQC binaries..."
ls -lh "$BIN_DIR"/pqc_host "$BIN_DIR"/*12345678*.ta "$BIN_DIR"/*12345678*.elf 2>/dev/null || true

# 5️⃣ Summary
echo
echo "✅ Contents of $BIN_DIR:"
ls -lh "$BIN_DIR" | grep -E 'pqc|tee|xtest|\.so|\.a'
echo
echo "✅ Number of xtest TAs copied:"
ls "$BIN_DIR/xtest_ta" | wc -l