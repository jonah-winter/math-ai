import math
def clamp16(x):
  return max(-32768, min(32768, x))
def q115(x):
  return clamp16(int(round(x * 32768.0)))
def main():
  filename = "lut_log.h"
  num_entries = 256
  with open(filename, "w") as f:
    f.write("#pragma once\n")
    f.write("#include <cstdint>\n\n")
    f.write(f"alignas(4) const int16_t lnLUT[{num_entries + 1}] = {{\n")
    entries = []
    for i in range(num_entries + 1):
      x = 0.5 + i / 256.0 * 0.5
      ln_val = math.log(x)
      to_q = q115(ln_val)
      entries.append(str(to_q))
    for i in range(0, len(entries), 8):
      line_chunk = ", ".join(entries[i : i + 8])
      if i + 8 < len(entries):
        f.write(f"    {line_chunk},\n")
      else:
        f.write(f"    {line_chunk}\n")
    f.write("};\n")
  print(f"Successfully generated {filename}")
if __name__ == "__main__":
  main()
