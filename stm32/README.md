# STM32 project notes

nanopb is integrated through the `Middlewares/Third_Party/nanopb` Git submodule and manual entries in `.cproject`. Regenerating code from `temp.ioc` may rewrite `.cproject` and remove the nanopb source-folder or include-path entries; after regeneration, verify that both Debug and Release still include nanopb.
