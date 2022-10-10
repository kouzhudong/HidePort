驱动获取网络连接信息

\\\\.\\Nsi
0x12001Bu
NsippDispatch
NsiEnumerateObjectsAllParametersEx
https://bbs.pediy.com/thread-257327.htm
https://bbs.pediy.com/thread-216334-1.htm
https://www.cctry.com/forum.php?mod=viewthread&tid=19233

"\\Device\\Tcp";
0x120003u  
TdxTdiDispatchDeviceControl->TdxTcpQueryInformationEx
IOCTL_TCP_QUERY_INFORMATION_EX
https://docs.microsoft.com/en-us/windows/win32/api/tcpioctl/ni-tcpioctl-ioctl_tcp_query_information_ex

注意:在vista上不要用XP上的办法.

获取的办法是:打开文件,然后发送ioctl即可,不必组装irp,更不必掉哦那个netio.sys的函数.
注意:不要获取驱动的名字,因为tcp6在XP和win7是由不同的驱动实现的.

所以也可拦截这些操作,从而实现隐藏端口,连接信息等功能.
拦截的办法,当然不推荐SSDT,也不推荐直接hook tcpip/netio的数据.
当然拦截查询底层的tcpip.sys/netio.sys里的数据是彻底的,但不稳定.
所以推荐的办法是附加设备,处理那些io操作即可.

15:32 2021/6/15


--------------------------------------------------------------------------------------------------


一种检查（获取本地）网络信息是否被HOOK，过滤，减少的思路：
1.第一次获取的Counter。这个一般不会减少。
  因为此步骤只有这个数，没有其他的信息，HOOK者无法判定要过滤的内容。
2.检查返回的内容与实际的个数是否一致。
  不一致，说明有过滤。
3.还有一些特殊的情况是增加信息。这个除外。

优点：无需驱动即可检查。
例如：GetTcp6Table2。


--------------------------------------------------------------------------------------------------
