/**
* 对配置文件进行读写操作
* 所有配置以 key - value pair 的形式存储
* 具体连接符和注释标号在 c 文件中定义
* @author	LiZhirui
*/

/**
* 写配置文件，如果文件已存在会清空原有数据
* 数据以 key - value pair 的形式存储
* 格式为 key=value
* 如果需要在某一 key - value pair 前添加注释
* 则在 annotationArray 对应项里添加
* @params	filePath		配置文件路径
* @params	keyArray		存放键的数组，与 valueArray 一一对应
* @params	valueArray		存放值的数组，与 keyArray 一一对应
* @params	annotationArray	存放注释的数组，与 key - value pair 一一对应，如果该键值对不需要注释，则对应项输入NULL
* @params	pairNum			key - value pair 的数量
* @return	成功返回0，否则返回1
*/
int writeConfigFile(char *filePath, char **keyArray, char **valueArray, char **annotationArray, int pairNum);
/**
* 读配置文件，将配置保存为字符串形式
* 读取如果读取时 value 长度超出了对应 bufsize - 1 ，则会自动截短
* 最后保留的是 value 字符串
* 会自动跳过空白和注释
* @params	filePath		配置文件路径
* @params	keyArray		存放键的数组，与 valueArray 一一对应
* @params	valueArray		存放值的数组，与 keyArray 一一对应
* @params	bufSizeArray	存放每个 value 缓冲区大小的数组
* @params	pairNum			key - value pair 的数量
* @return	成功返回0，否则返回1
*/
int readConfigFile(char *filePath, char **keyArray, char **valueArray, int *bufSizeArray, int pairNum);