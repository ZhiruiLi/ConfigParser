/**
* 对配置文件进行读写操作
* 所有配置以 key - value pair 的形式存储
* 具体连接符和注释标号在 c 文件中定义
* @author	LiZhirui
*/

#include <stdio.h>
// key - value pair 之间的连接符
#define LINK_SYMBOL			"="
// 注释标识符
#define ANNOTATION_SYMBOL	"#"
// 判定是否为空白符
#define IS_SPACES(x)			( ' ' == (x) || '\t' == (x) || '\n' == (x) || '\r' == (x) || '\f' == (x) || '\b' == (x) )

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
int writeConfigFile(char *filePath, char **keyArray, char **valueArray, char **annotationArray, int pairNum){
	FILE *fp = fopen(filePath, "w");
	if (NULL == fp){
		return 1;
	}
	int i = 0;
	for (; i<pairNum; i++){
		if (NULL != annotationArray[i]){
			fwrite(ANNOTATION_SYMBOL, sizeof(char), strlen(ANNOTATION_SYMBOL), fp);
			fwrite(annotationArray[i], sizeof(char), strlen(annotationArray[i]), fp);
			fwrite("\n", sizeof(char), 1, fp);
		}
		fwrite(keyArray[i], sizeof(char), strlen(keyArray[i]), fp);
		fwrite(LINK_SYMBOL, sizeof(char), strlen(LINK_SYMBOL), fp);
		fwrite(valueArray[i], sizeof(char), strlen(valueArray[i]), fp);
		fwrite("\n", sizeof(char), 1, fp);
	}
	fclose(fp);
	fp = NULL;
	return 0;
}

/**
* 跳过注释和空白
* @params	fpp		文件指针的指针
* @return	成功返回0，否则返回1
*/
int _skipAnnotationAndSpaces(FILE **fpp){
	char temp[1] = { ' ' };
	int charSize = sizeof(char);
	int annotationHeadLen = strlen(ANNOTATION_SYMBOL);
	char *annotationHead = malloc(annotationHeadLen + 1);
	if (NULL == annotationHead){
		return -1;
	}
	int iResult;
	int count = 0;
	while (1){
		while (IS_SPACES(temp[0])){
			iResult = fread(temp, charSize, 1, *fpp);
			if (0 == iResult){
				free(annotationHead);
				annotationHead = NULL;
				return count;
			}
			count += 1;
		}
		fseek(*fpp, 0 - charSize, SEEK_CUR);
		count -= 1;
		iResult = fread(annotationHead, charSize, annotationHeadLen, *fpp);
		if (annotationHeadLen != iResult){
			fseek(*fpp, 0 - iResult*charSize, SEEK_CUR);
			free(annotationHead);
			annotationHead = NULL;
			return count;
		}
		annotationHead[annotationHeadLen] = '\0';
		if (0 != strcmp(annotationHead, ANNOTATION_SYMBOL)){
			fseek(*fpp, 0 - iResult*charSize, SEEK_CUR);
			free(annotationHead);
			annotationHead = NULL;
			return count;
		}
		count += iResult;
		while (1){
			iResult = fread(temp, charSize, 1, *fpp);
			if (0 == iResult){
				free(annotationHead);
				annotationHead = NULL;
				return count;
			}
			count += 1;
			if ('\n' == temp[0]){
				break;
			}
		}
		iResult = fread(temp, charSize, 1, *fpp);
		if (0 == iResult){
			free(annotationHead);
			annotationHead = NULL;
			return count;
		}
		count += 1;
	}
}
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
int readConfigFile(char *filePath, char **keyArray, char **valueArray, int *bufSizeArray, int pairNum){
	FILE *fp = fopen(filePath, "rb");
	if (NULL == fp){
		return 1;
	}
	int charSize = sizeof(char);
	int linkSymbolLen = strlen(LINK_SYMBOL);
	int i = 0;
	int j = 0;
	int isEnd = 0;
	int lineEnd = 0;
	char temp[1];
	int iResult = 0;
	int maxKeyLen = 0;
	int tempLen;
	char *tempKeyBuf;
	char *tempLinkBuf;
	for (i = 0; i < pairNum; i++){
		tempLen = strlen(keyArray[i]);
		if (tempLen > maxKeyLen){
			maxKeyLen = tempLen;
		}
	}
	tempKeyBuf = malloc(maxKeyLen + 1);
	tempLinkBuf = malloc(linkSymbolLen + 1);
	if (NULL == tempKeyBuf || NULL == tempLinkBuf){
		fclose(fp);
		free(tempKeyBuf);
		tempKeyBuf = NULL;
		free(tempLinkBuf);
		tempLinkBuf = NULL;
		return 1;
	}
	// 每一次循环都拿读到的key与key数组里所有的值进行比对，直到读完整个文件
	while (!isEnd){
		_skipAnnotationAndSpaces(&fp);
		for (i = 0; i < pairNum; i++){
			iResult = fread(tempKeyBuf, charSize, strlen(keyArray[i]), fp);
			if (0 == iResult){
				isEnd = 1;
				break;
			}
			tempKeyBuf[iResult] = '\0';
			if (strlen(keyArray[i]) != iResult || 0 != strcmp(tempKeyBuf, keyArray[i])){
				fseek(fp, 0 - charSize*iResult, SEEK_CUR);
				continue;
			}
			// 如果相等，就要判断下面是否为连接符，防止出现同前缀的情况
			_skipAnnotationAndSpaces(&fp);
			iResult += fread(tempLinkBuf, charSize, linkSymbolLen, fp);
			tempLinkBuf[linkSymbolLen] = '\0';
			if (0 != strcmp(tempLinkBuf, LINK_SYMBOL)){
				fseek(fp, 0 - charSize*iResult, SEEK_CUR);
				continue;
			}
			_skipAnnotationAndSpaces(&fp);
			valueArray[i][bufSizeArray[i] - 1] = '\0';
			for (j = 0; j < bufSizeArray[i] - 1; j++){
				iResult = fread(&valueArray[i][j], charSize, 1, fp);
				if (0 == iResult){
					valueArray[i][j] = '\0';
					isEnd = 1;
					break;
				}
				// 如果读到了换行就查看前面是否有回车，如果有就将回车设为结尾，否则将换行设为结尾
				if ('\n' == valueArray[i][j]){
					lineEnd = 1;
					if ('\r' == valueArray[i][j - 1]){
						valueArray[i][j - 1] = '\0';
						break;
					} else{
						valueArray[i][j] = '\0';
						break;
					}
				}
			}
			if (!lineEnd){
				while (1){
					iResult = fread(temp, charSize, 1, fp);
					if (0 == iResult || '\n' == temp[0]){
						break;
					}
				}
			} else{
				lineEnd = 0;
			}
		}
	}
	fclose(fp);
	free(tempKeyBuf);
	tempKeyBuf = NULL;
	free(tempLinkBuf);
	tempLinkBuf = NULL;
	return 0;
}