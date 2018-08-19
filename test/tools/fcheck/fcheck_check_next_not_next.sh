# RUN: bash %s

# CHECK: hello
echo hello

echo middle

# CHECK-NEXT: world
echo world
