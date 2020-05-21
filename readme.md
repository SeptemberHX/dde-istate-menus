## dde-istate-menus

IStat menus on macOS alternative for DDE.

> This software has nothing to do with the Istat menu excepting adopting its UI design
> The main part of the system monitoring code in `utils` comes from [deepin-system-monitor](https://github.com/linuxdeepin/deepin-system-monitor)

> Still in development

### Screenshot

#### Network

![](./screenshots/network.png)


### Attention

```
sudo setcap cap_net_raw,cap_dac_read_search,cap_sys_ptrace+ep `which dde-dock`
```