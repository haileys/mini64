# Memory Map

| First address           | Last address            | Purpose                            |
|-------------------------|-------------------------|------------------------------------|
| `0x0000_0000_0000_0000` | `0x0000_7fff_ffff_ffff` | Userland virtual address range     |
| `0xffff_ff00_0000_0000` | `0xffff_ff00_0000_1fff` | Kernel code + data                 |
| `0xffff_ff00_001f_d000` | `0xffff_ff00_001f_dfff` | Kernel temporary mapping region    |
| `0xffff_ff00_001f_e000` | `0xffff_ff00_001f_efff` | Kernel stack guard (unmapped)      |
| `0xffff_ff00_001f_f000` | `0xffff_ff00_001f_ffff` | Kernel stack during initialization |
| `0xffff_ff00_1000_0000` | `0xffff_ff00_1fff_ffff` | Kernel dynamic allocation zone     |
| `0xffff_ff80_0000_0000` | `0xffff_ffff_bfff_ffff` | Recursively mapped PML1 tables     |
| `0xffff_ffff_c000_0000` | `0xffff_ffff_ffdf_ffff` | Recursively mapped PML2 tables     |
| `0xffff_ffff_c000_0000` | `0xffff_ffff_ffff_efff` | Recursively mapped PML3 tables     |
| `0xffff_ffff_ffff_f000` | `0xffff_ffff_ffff_ffff` | Recursively mapped PML4 table      |
