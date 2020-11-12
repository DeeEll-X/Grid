# Grid
## 0.0.4
- support 'Grid start containerId'
  - currently use terminal, args, cwd, envs
  - use clone to isolate namespace
  - pivot root
  - mount proc(/proc), tmpfs(/dev)
- support 'Grid kill containerId signal'
## 0.0.3
- support 'Grid create containerId bundle'
  - leverage aufs to add mnt URL and root URL
- GRID_CONFIG implies config file path
- config file records rootdir, the data directory of Grid, storing containers, images.

## 0.0.2

- import boost program_option @Veiasai

## 0.0.1

- initialize directory @DeeEll-X