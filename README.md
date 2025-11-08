# booth_multiplier
Booth算法是一种适合于通过硬件实现的简便算法。

大概步骤：
- 对其中乘数做Booth编码
- 根据Booth编码生成部分积 （乘法变成移位&加法）
- 将部分积通过华莱士压缩树进行压缩 （CSA并行计算）
- 将华莱士压缩出的最后的carry和sum做全加

具体原理参考《FAST MULTIPLICATION ALGORITHMS AND IMPLEMENTATION.pdf》