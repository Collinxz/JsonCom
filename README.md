# JsonCom

* c语言实现的Json压缩和解压
* 依赖cJSON词法解析器
* 目前只支持Json文件的value值是字符串
* **对于小文件压缩效率不好**
* example:
** 压缩前:  
{  
    "Alert": [  
        {  
            "type": "KBMouse",  
            "data": "LOC_OPTIONS_KEY_V",  
            "information":{  
                "name": "xuzhi",  
                "number": "123456789"  
            }  
        }  
    ],  
    "Attack": [  
        {  
            "type": "KBMouse",  
            "data": "LOC_OPTIONS_KEY_A",  
            "dream": "superman"  
        }  
    ],  
    "AutoExplore": [  
        {  
            "type": "KBMouse",  
            "data": "LOC_OPTIONS_KEY_E"  
        },  
        {  
        	"type": "KBCat",  
			    "data": "LOC_OPTIONS_KEY_C"   
		    }  
    ],  
    "AutoFind": "test"  
}  
** 压缩后  
{"t":[[0,"Alert","Attack","AutoExplore","AutoFind"],[0,"type","data"],[2,"information"],[2,"dream"],[0,"name","number"]],  
 "v":[1,[0,[3,"KBMouse","LOC_OPTIONS_KEY_V",[5,"xuzhi","123456789"]]],[0,[4,"KBMouse","LOC_OPTIONS_KEY_A","superman"]],[0,[2,"KBMouse","LOC_OPTIONS_KEY_E"],[2,"KBCat","LOC_OPTIONS_KEY_C"]],"test"]}  
