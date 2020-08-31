# JsonCom

## 介绍
使用c语言实现的Json压缩和解压  
依赖cJSON词法解析器   
目前仅支持Json文件中的value值是字符串  
**对于小文件压缩效率不好**

## 样例
压缩前: 
```json
{  
    "cats": [  
        {  
            "type": "animal",  
            "data": "it is cat",  
            "information":{  
                "name": "eryue",  
                "number": "1234567890"  
            }  
        }  
    ],  
    "dogs": [  
        {  
            "type": "animal",  
            "data": "it is dog",  
            "dream": "clever"  
        }  
    ],  
    "human": [  
        {  
            "type": "animal",  
            "data": "it is animal"  
        },  
        {  
        	"type": "human",  
			"data": "he is human"   
		}  
    ],  
    "lion": "Nothing"  
}  
```

压缩后:
```json
{
    "t":    [[0, "cats", "dogs", "human", "lion"], [0, "type", "data"], [2, "information"], [2, "dream"], [0, "name", "number"]],
    "v":    [1, [0, [3, "animal", "it is cat", [5, "eryue", "1234567890"]]], [0, [4, "animal", "it is dog", "clever"]], [0, [2, "animal", "it is animal"], [2, "human", "he is human"]], "Nothing"]
}
```
