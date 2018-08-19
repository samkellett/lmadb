# RUN: bash %s
# RUN: bash -e %s

# CHECK-NEXT: hello
echo hello

# CHECK-NEXT: world
echo world

echo something

# CHECK: else
echo else

echo unchecked
