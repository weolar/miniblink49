
#include "RootWindow.h"
#include "RegWnd.h"
#include "Resource.h"
#include "HideWndHelp.h"

#include <windows.h>
#include <Shlwapi.h>
#include <xstring>
#include <vector>
#include <functional>
#include <process.h>
#include <wininet.h>
#include <ShlObj.h>

std::string getUrl();
#define MAX_URL_LENGTH 255

std::string utf16ToUtf8(LPCWSTR lpszSrc)
{
    std::string sResult;
    if (lpszSrc != NULL) {
        int  nUTF8Len = WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
        char* pUTF8 = new char[nUTF8Len + 1];
        if (pUTF8 != NULL) {
            ZeroMemory(pUTF8, nUTF8Len + 1);
            WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, pUTF8, nUTF8Len, NULL, NULL);
            sResult = pUTF8;
            delete[] pUTF8;
        }
    }
    return sResult;
}

std::wstring utf8ToUtf16(const std::string& utf8)
{
    std::wstring utf16;
    size_t n = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), nullptr, 0);
    if (0 == n)
        return L"";
    std::vector<wchar_t> wbuf(n);
    ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], n);
    utf16.resize(n + 5);
    utf16.assign(&wbuf[0], n);
    return utf16;
}

std::string getUrl()
{
    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    memset(&path.at(0), 0, sizeof(wchar_t) * (MAX_PATH + 1));

    ::GetModuleFileNameW(nullptr, &path.at(0), MAX_PATH);
    ::PathRemoveFileSpecW(&path.at(0));
    ::PathAppendW(&path.at(0), L"node.exp");

    std::wstring probPath(&path.at(0), wcslen(&path.at(0)));
    BOOL isExist = ::PathFileExistsW(probPath.c_str());

    std::string urlA = utf16ToUtf8(L"file:///G:/test/pakage/linux/我/v8_bug.html");
    //urlA = utf16ToUtf8(L"file:///G:/test/web_test/单病种/单病种质量管理系统.html");
    //urlA = utf16ToUtf8(L"file:///D:/我Storage/selectone.html");

    const char* url = //urlA.c_str();"
        "http://www.mincuan.com.cn/audio.html"; // 有个按钮
        "http://www.ip168.com/";
        "http://f10.legu168.com:81/gu/600000/0001.htm";
        "file:///G:/test/web_test/iphey/main.htm";
        "file:///G:/test/web_test/QQ2700433330/res/ui/window_main/index.html";
        "http://192.168.31.251:5678/jq22/mp4.htm";
        "https://online.lifeliqe3d.cn/app/scene/p_geol_hexagonalni_pyramida_papercut?jwt=eyJhbGciOiJSUzUxMiIsInR5cCI6IkpXVCJ9.eyJuYmYiOjE2NzYzNTYyMjksImV4cCI6MTY3NzIyMDIyOSwiaXNzIjoiTExRIiwiYXVkIjoiTWVuY28ifQ.R3-a5IQiVKIY4EG7ga-WHkjxvK5tngiWxo86tCzlWg7cHeVFqyxOj7qrpRWOqnm5RUNKPr_623x58UEIdDy2e7MjochU0IVWymUT_fajM2CjZYMe1KttO92Dl_roRzGwc7pQaBQhzLlps85iiaM7kGvrzxspOTvVcBmI8klUe9zhG74Gsr-DQTuDDibKXfwkzio6H5E2uO7oav3LL2fPkchGtNBQn7ErAg-FRCf2xy9k070KC0SN2jXhNTSDXLB0xWWy3PExFfM3DKsIRnpqaDMq2Kgg24Szn3Khavjs3uDf1OwlCqd8GAWOPQ9GTIL4UdkwF29NU1GWeiWjU3Z5tA";

        "http://39.103.236.166:16217/medical-web/industryMain.do?method=displayH5&ciphertext=1b82c6ea66decfcd7e176f2e20dc09f160a03e979d037e48f1381d4eab3e9d4b96e5efc87f72f3b6b2c25c050c159646af90dc66180c51dee12f6734de909d622cbc7b87ea86283456179987a65d815d0a6b5af9756a3f76bb4c692ef2d5436e789db2e455a6559294571893439020c64323eedc90601d4d23f87c840b41f81dc708cfbf7c76e94052d9bfc5bffb762b1f674659ccc70a6ceb2971b10db84f485bca69433565f9061f68789ce998a96cad5587aa061c2991870bbac80e33f581f9133f1c2ff4993d55e44654e40d89cea5a705792f2f7e182f11074ea348990a8f52cdbaae68e2ace8f1620e3f2479f6f93ef04b93143a359b835bccff7a59b8ac1efb9daaf6271bfdf8ba46d7e1a6231f7ab5b63fa8ec209cb80f14e5a351f7";
        "http://39.103.236.166:16217/medical-web/industryMain.do?method=displayH5&ciphertext=1b82c6ea66decfcd7e176f2e20dc09f160a03e979d037e48f1381d4eab3e9d4b96e5efc87f72f3b6b2c25c050c159646af90dc66180c51dee12f6734de909d622cbc7b87ea86283456179987a65d815d0a6b5af9756";
        "http://39.103.236.166:16217/medical-web/industryMain.do?method=displayH5&ciphertext=1b82c6ea66decfcd7e176f2e20dc09f160a03e979d037e48f1381d4eab3e9d4b96e5efc87f72f3b6b2c25c050c159646af90dc66180c51dee12f6734de909d622cbc7b87ea86283456179987a65d815d0a6b5af9756a3f76bb4c692ef2d5436e789db2e455a6559294571893439020c64323eedc90601d4d23f87c840b41f81dc708cfbf7c76e94052d9bfc5bffb762b1f674659ccc70a6ceb2971b10db84f485bca69433565f9061f68789ce998a96cad5587aa061c2991870bbac80e33f581f9133f1c2ff4993d55e44654e40d89cea5a705792f2f7e182f11074ea348990a8f52cdbaae68e2ace8f1620e3f2479f6f93ef04b93143a359b835bccff7a59b8ac1efb9daaf6271bfdf8ba46d7e1a6231f7ab5b63fa8ec209cb80f14e5a351f7";

        "file:///G:/test/web_test/pdf_print/main3.htm";
        "file:///G:/test/web_test/media_match.htm";

        "222.92.30.62:18182";
        "file:///G:/test/web_test/copy_bug/HTML_WKE/Win32/Debug/editor/index.html";
        "file:///G:/test/web_test/mov_test/v.html";
        "https://passport.hupu.com/v2/login#/";
        "https://developer.fengmap.com/fmAPI/help-fmdemo.html#PolygonMarker"; // webgl crash
        "https://wallpaper.zhhainiao.com/3012/dynamicWallpaper";
        "https://cc.163.com/984606/";
        "file:///D:/test/web/cc_163/index.htm";// 网易cc直播排版问题
        "file:///C:/Users/Administrator/Desktop/TT0.pdf";
        "file:///G:/test/web_test/jq22/mp4.htm";
        "222.92.30.62:8182"; // 链接断了
        "https://www.kanxue.com/book-30-226.htm";
        "file:///C:/Users/Administrator/Desktop/1.pdf";
        "file:///G:/test/web_test/AudioContextTest.html";
        "file:///G:/test/web_test/AIDA64_RemoteSensor/SC.htm"; // gif放久了很卡
        "file:///G:/test/web_test/QQ806467874(xing)/index.html"; // vip版本有延迟

        "file:///G:/test/web_test/longshiyun/index.html#/pricepack/xxj";
        "file:///G:/test/web_test/alen_bug/index.html";
        "https://xsdt1.i-xinnuo.com/?param="; // 下拉框出不来
        "file:///G:/test/web_test/alen_bug/index.html#";


        "http://192.168.222.1:8000/jieshao.htm";
        "https://www.chinaacc.com/wangxiao/tanchu/jieshao.shtml";
        "https://www.cdeledu.com/";
        "file:///G:/test/web_test/icoc/video.htm";
        "https://www.51miz.com/sound/?utm_term=5397999&utm_source=baidu2&bd_vid=8511794874001637942";
        "file:///C:/Users/Administrator/Documents/input.htm";
        "https://tongbu.eduyun.cn/tbkt/tbkthtml/wk/weike/2020QJ/2020QJ01YWTB001.html"; // 会崩溃
        "file:///G:/test/web_test/cef_silent_print/test.html.pdf";
    "file:///G:/test/web_test/CallNumber/testSocket.html";
    "file:///G:/test/web_test/test(3).html";
    "http://zj3d2-screen.zhihuipk.com/#/ztindex";
    "https://www.bilibili.com/video/BV1rp4y1n7Ft?spm_id_from=333.851.b_7265636f6d6d656e64.1";

    "http://aola.100bt.com/h5/";
    "https://www.yuque.com/yuque/ng1qth/dashboard";
    "file:///P:/test/web_test/chrome-tabs-gh-pages/index.html";
    "taobao.com";
    "http://static.newayz.com/sv/videoblink.html";
    "https://yunpan.360.cn/file/index#/fileManage/my/file/%2F";
    "file:///G:/test/exe_test/Test53kf/test.html";
    "file:///G:/test/web_test/zzz_test/queen.htm";
    "https://exhibition.trinasolar.com/index.html#/page/battery/SQ";
    "file:///G:/test/web_test/gamebox_A.html";
    "http://demo.finebi.com//decision/v10/entry/access/115e7870-8b9a-4e33-b7ff-6165041e9fab?dashboardType=4&width=1610&height=787";
    "file:///G:/test/web_test/main_copy.html";
    "http://192.168.9.240:13000/#/auth/audit";
    //"http://www.suduzy8.com:777/play-14848.html";
    "http://192.168.9.240:13000/#/user/login";
    "file:///G:/test/web_test/test_download.htm";
    "file:///M:/doc/fingerprint/testfp/gongjux.htm";
    "https://www.mgtv.com/b/349988/11411698.html?fpa=15801&fpos=5&lastp=ch_home";
    "http://demo.finebi.com/"; //v875不能点击里面按钮
    "http://demo.gin-vue-admin.com/#/login";
    "https://haokan.baidu.com/v?vid=12462553566250398173&pd=bjh&fr=bjhauthor&type=video";
    "https://v.youku.com/v_show/id_XMzUzNjQzNjQ3Mg==.html";
    "file:///P:/test/web_test/test_js_timer.html";
    "https://tv.cctv.com/live/cctv7";
    "https://new.qq.com/omn/20210310/20210310A089KB00.html";
    "http://192.168.0.103:8000/hls_deme/hls.htm";
    "https://hls-js.netlify.app/demo/";
    "http://192.168.0.103:8000/wasm/canvasDemo-wasm1/random.html";
    "http://192.168.0.103:8000/bufferAll.html";

    "file:///G:/test/web_test/audio_test/Untitled1.html";
    "file:///G:/test/web_test/audio_test/webtest.htm";
    "https://chowsangsang.tmall.com";
    "https://tarpa.tmall.com/wow/ark-pub/common/156bb3b2/tpl?wh_sid=ac45106ffe692a4b";
    "file:///G:/test/web_test/hetao_mp4/test.htm";
    "http://192.168.56.1:8000/layui-soul-table.htm#/zh-CN/component/changelog";
    "http://192.168.56.1:8000/ajax.htm";

    "http://221.226.213.42:5055/dist/index.html#/businessTown"; // 动画卡慢
    "file:///G:/test/web_test/test_printer/testPrint.html";
    "file:///G:/test/web_test/baidupan/html/land.html";
    "file:///G:/test/web_test/baidupan/try_win_focus.htm";
    "http://www.aardio.com/";
    "https://www.layui.com/demo/table/parseData.html"; // 测试下载

    "https://www.animejs.cn/documentation/#linearEasing";
    "http://02.build.kelexuexi.com";
    "https://www.aixuebanban.com/teacher/filemanager";
    "https://www.w3school.com.cn/tiy/t.asp?f=html5_draganddrop";
    "https://www.zhihu.com/zvideo/1285349052423831552";
    "file:///G:/test/web_test/niushibang/index.htm";
    "https://meet.jit.si";
    "http://applicationteam.gitee.io/nsb-classroom-doc/miniblink_test/";
    "file:///g:/test/web_test/icoc/video.htm";
    "weibo.com";
    "file:///G:/test/web_test/icoc/sina/sina.htm";

    "https://www.zhihu.com/question/415822945";
    "http://192.168.71.1:8000/video.htm";
    "http://192.168.71.1:8000/ea0/index.htm";
    "https://ea0.com";
    "file:///G:/mycode/mbvip/out/x86/Release/1234.htm";
    "http://aola.100bt.com/play/play.html";
    "https://weibo.com/2970452952/JeQiWDULX?type=comment";
    "https://new.qq.com/omn/20200727/20200727A0J6LI00.html";
    "https://weibo.com/tv/show/1034:4531439128215563";

    "file:///G:/test/web_test/openlayers/image.html";
    "file:///G:/test/web_test/icoc/weibo.htm";

    "http://video.sina.com.cn/p/ent/doc/2018-02-07/101868002478.html";

    "miniblink.net";
    "file:///G:/test/web_test/test_frame/iframe.html";
    "file:///G:/mycode/easyckl/scripts/test_mbquery.htm";
    "file:///G:/cache/qqcache/93527630/filerecv/m17090_802001_cor_insert.html";
    "https://live.bilibili.com/10339607";

    "file:///G:/test/web_test/xingzhihai/test.html";
    "file:///G:/test/web_test/icoc/video.htm";

    "file:///G:/test/web_test/58/test.html";
    "http://1.205.23.121:7418/";
    "http://zhilian.hitecloud.net/workappraise/#/web/home?mtoken=34331ad9c408630e6ab67adb76983b01";
    "http://xyx.yulebuyu.com/140/";
    "file:///G:/test/web_test/tuoshao/bad/index.html";

    "file:///G:/test/web_test/58/test.html";
    "file:///G:/test/web_test/tuoshao/index.html";
    "kg.kuan-gu.com/html/1//203/204/list-3.html";

    "file:///G:/test/web_test/58/wbrTest.html";
    "file:///G:/cache/qqcache/93527630/filerecv/wbrtest.html";
    "file:///G:/test/web_test/jq22/index.html";

    "file:///G:/test/exe_test/HongheQtDemo/Bin/MathSubject/DyCourseware/GeoGebra/HTML5/5.0/previewggb.html";

    "file:///G:/test/web_test/jq22/index.html";
    "https://www.geogebra.org/m/ahfyauh3";

    "file:///F:/test/web_test/webgl.htm";
    "http://www.wangeditor.com/";
    "https://mms.pinduoduo.com/chat-windows/index.html#/";

    "https://nl.hotels.com/";
    "file:///E:/mycode/wke-master/demo/bin/vlc.html";
    "file:///G:/test/web_test/geogebra-math/GeoGebra/HTML5/5.0/3d-template.html";
    "file:///G:/test/exe_test/honghe_20200324/MathSubject/DyCourseware/GeoGebra/HTML5/5.0/previewggb.html";
    "http://hepfw.etwowin.com.cn/";
    "mbpack:///1.htm";
    "http://windbg.org/";
    "file:///G:/test/exe_test/HHBlinkTest/Environment/open_new_page.htm";
    "file:///G:/test/exe_test/HHBlinkTest/Environment/MathSubject/DyCourseware/GeoGebra/HTML5/5.0/previewggb.html";
    "pan.baidu.com";
    "file:///G:/test/web_test/testlayer/index.html";
    "https://blog.csdn.net/u011882998/article/details/51034147";
    "file:///G:/mycode/PDB-Downloader.git/trunk/download.htm";
    "http://m.baidu.com/s?word=12306&sa=ts_1&ts=0&t_kt=0&ie=utf-8&rsv_t=9d07v4i2NAoIydVWMT3Bg8hA4nnRMWPPXPan9MMHwW3UAb2FzJrJ&rsv_pq=11524657842255675396&ss=100&sugid=102897658533638957&rq=12";
    "http://www.4399.com/flash/188528.htm";

    "file:///G:/test/web_test/icoc/echars.htm";
    "https://www.echartsjs.com/zh/index.html";
    "https://browserleaks.com/canvas";
    "http://www.nike.com.hk/";
    "https://sheetjs.com/demo/table.html";
    "file:///G:/test/exe_test/sengyi/css3_transition.htm";
    "https://d6.hbyskc888.com/";
    "https://www.layui.com/demo/layer.html";
    "file:///G:/test/web_test/pure-css3-mac-dock/css3.htm";

    //"file:///G:/test/web_test/dashgame/index2.htm";
    //"G:\\test\\web_test\\dashgame\\index.htm";
    "http://fontawesome.dashgame.com/";
    "file:///G:/test/web_test/dashgame/index.htm";

    "file:///G:/test/web_test/jianglv_test/test.html";
    "https://api.weibo.com/oauth2/authorize?redirect_uri=https%3A%2F%2Fudb3lgn.huya.com%2Fweb%2Fv2%2Fcallback&client_id=4136543982&state=%7B%22action%22%3A%221%22%2C%22appid%22%3A%225002%22%2C%22busiurl%22%3A%22https%3A%2F%2Fwww.huya.com%22%2C%22byPass%22%3A3%2C%22domain%22%3A%22huya.com%22%2C%22domainList%22%3A%22%22%2C%22exchange%22%3A%2242135901%22%2C%22guid%22%3A%22d7c2841990b84019b5544fd3af4dd402%22%2C%22remember%22%3A0%2C%22requestId%22%3A%2242137148%22%2C%22smid%22%3A%22%22%2C%22style%22%3A%221%22%2C%22terminal%22%3A%22web%22%2C%22type%22%3A%22wb%22%2C%22uri%22%3A30003%2C%22win%22%3A%221%22%7D&display=default";
    "file:///G:/test/web_test/webgl/earth/index.htm";
    "http://webglsamples.org/";

    "https://www.html5tricks.com/demo/js-passwd-generator/index.html";
    "https://mms.pinduoduo.com/chat-merchant/index.html?r=0.6672943236250943";
    "news.baidu.com";
    "https://h5.919flying.com/#/play?band=%7B%22game_id%22%3A%22ee6001ee266c4075b2b5ba931996f333%22%7D";
    "file:///G:/test/web_test/index_flash/test.htm";
    "http://103.42.78.59/lawreg/login.jspx";

    "file:///G:/test/exe_test/sengyi/css3_transition.htm";
    "https://auth.alipay.com/login/index.htm";
    "file:///G:/test/web_test/ueditor/video.htm";
    "http://wz99.top:8096/web/index.html#!/itemdetails.html?id=f7f9d451896ebafde2ffedab8caa9da6&serverId=bf3aff3b3b374884a36e8b7bee179021  ";

    "http://wz99.top:8096/web/index.html#!/itemdetails.html?id=f7f9d451896ebafde2ffedab8caa9da6&serverId=bf3aff3b3b374884a36e8b7bee179021";

    "file:///G:/test/web_test/test_printer/print-test1.html";
    "https://mj19916992-4.icoc.vc/";
    "http://www.xunyou.com/client/2013/jump.php?action=3&eid=54&appType=0001&ver=3.105.242.19111413&nickname=&username=bowengf&viptype=1&userid=bowengf&spid=0000&saleid=0&pwd=1BB1D88D646E08217015E5D605ED&pwd2=4098EDDD5036D969B56B5D228BC9659503B45194&pwd3=930FE476C61BD6E9596388BD6CBBD4A158415ECB217D49AA672F77CCFB32F016&spid2=&vc=&SACC=9D588A6EB0E68C462D74E7720E012673&xunyouvip=1&gameid=23646&gameareaname=BEF8B5D8C7F3C9FA2D537465616D2FD1C7B7FE&edate=0000:2011-01-01%2000:00:00";

    "https://image.baidu.com/search/detail?ct=503316480&z=0&ipn=d&word=%E4%BB%8A%E6%97%A5%E6%96%B0%E9%B2%9C%E4%BA%8B&step_word=&hs=0&pn=3&spn=0&di=124960&pi=0&rn=1&tn=baiduimagedetail&is=0%2C0&istype=0&ie=utf-8&oe=utf-8&in=&cl=2&lm=-1&st=undefined&cs=2192019847%2C875856785&os=171449302%2C3049960527&simid=3446915372%2C414806786&adpicid=0&lpn=0&ln=1250&fr=&fmq=1573009882761_R&fm=&ic=undefined&s=undefined&hd=undefined&latest=undefined&copyright=undefined&se=&sme=&tab=0&width=undefined&height=undefined&face=undefined&ist=&jit=&cg=&bdtype=0&oriquery=&objurl=http%3A%2F%2Fs06.lmbang.com%2FM00%2FB6%2FB6%2FecloD1vW3amAdUnnAAE-70Avd8k657.jpg&fromurl=ippr_z2C%24qAzdH3FAzdH3Fooo_z%26e3Bsw4wkwg2_z%26e3Bv54AzdH3Fp5rtvAzdH3Ft1-nd0bbc0b_z%26e3Bip4s&gsm=&rpstart=0&rpnum=0&islist=&querylist=&force=undefined";
    "file:///C:/cache/93527630/FileRecv/test_tip_f7.html";
    "file:///G:/test/web_test/qq_login.htm";
    "https://m.baidu.com/s?word=%E8%B0%B7%E6%AD%8C%E5%AE%9E%E7%8E%B0%E9%87%8F%E5%AD%90%E9%9C%B8%E6%9D%83&ssid=0&from=0&pu=sz%40224_220%2Cta%40iphone___24_74.0&qid=1135683139&rqid=1135683139&sa=iks_hot_1&ss=000000000011";
    "http://103.91.208.239:89/index.html?host=103.91.208.239&port=10892&userid=68110111&password=828b49bc4a7d9f8284f5c08a4bd62038&homepage=0&helppage=1&useexmlskin=0";
    "file:///C:/cache/93527630/filerecv/v.html";
    "https://demo.fastadmin.net/admin/auth/admin?ref=addtabs";
    "file:///G:/test/web_test/ggzuhao/test.htm";
    "https://www.canadasuperdraw.com/canada30s/";
    "http://www.speedtest.cn/";
    "https://diygod.me/";
    "https://aplayer.js.org/#/";
    "file:///G:/test/web_test/APlayer-master/test/audio.htm";
    "file:///G:/test/web_test/APlayer-master/demo/index2.html";
    "file:///G:/test/web_test/test-iview-select.htm";
    "https://www.tiny.cloud/docs/demo/full-featured";
    "http://yun2.anyicw.com/logallg.html";
    "https://omsweb-1256831280.cos.ap-shanghai.myqcloud.com/webui/page5_xunjiadan/main.html";
    "https://www.w3school.com.cn/tiy/t.asp?f=html_select";
    "https://s.taobao.com/search?spm=a21bo.2017.201856-fline.4.5af911d97Ns7Gl&q=%E5%A5%B3%E9%9E%8B&refpid=420466_1006&source=tbsy&style=grid&tab=all&pvid=d0f2ec2810bcec0d5a16d5283ce59f68";

    "file:///C:/cache/93527630/FileRecv/Outlines.pdf";
    "file:///C:/cache/93527630/filerecv/table_test1.html";
    "file:///G:/test/web_test/test_video.html";

    "file:///G:/test/web_test/table2excel/index.htm";
    "http://www.w3school.com.cn/tiy/t.asp?f=html_a_download";
    "http://report.msunsoft.cn:7010/MSunReportWeb/leadersearch?userSysId=90";
    "https://mms.pinduoduo.com/home/";
    "https://easy.lagou.com/utrack/trackMid.html?f=https%3A%2F%2Feasy.lagou.com%2Fcan%2Findex.htm&t=1561686355&_ti=136";
    "http://test.diantaolianmeng.com/test/1.html";
    "http://192.168.0.101:8000/1559721804506.html";
    "file:///F:/test/web_test/msunsoft2/input.htm";
    "https://imsp.msunsoft.com:1443/msun-imsp/";
    "https://rbt.guorenpcic.com/ecar/#/queryRoot/list";
    "http://www.jszotye.com:33893/myhtm/itemchaxun.htm";
    "https://pei.yay.com.cn";
    "https://login.taobao.com/member/login.jhtml?sub=true&style=miniall&from=subway&full_redirect=true&newMini2=true&tpl_redirect_url=//subway.simba.taobao.com/entry/login.htm";
    "file:///F:/test/flexbox.htm";
    "https://printserver.bpic.com.cn:8080/WebReport/ReportServer?reportlet=toubaodanBBJ.cpt&op=view&p2=5110003602019007069&username=xiaoguan&password=123456";
    "https://www.linkedin.com";
    "file:///C:/cache/93527630/FileRecv/jstest.html";
    "http://www.jq22.com/yanshi21020";
    "https://www.baidu.com/link?url=xmSTrf7uh78HUemcDIdECCP6T3eVveCKm0ugRsoBsHUyEvX-g5tYUY9rfPJAhuUFymmgTInRUE6xD5RsrVZknK&wd=&eqid=ca1970730017f297000000025cc6c8e3";
    "file:///E:/test/jQueryPrint/test_print_7.htm";
    "http://qqvip.xunyou.com/client/2014/194.shtml";
    "http://www.w3school.com.cn/tiy/t.asp?f=html5_video_autoplay";
    "file:///E:/test/baidu/index_test_mp3.html";
    "https://rbt.guorenpcic.com/ecar";
    "file:///C:/cache/93527630/FileRecv/test(1).html";
    "file:///C:/cache/93527630/filerecv/touchtest.html";
    "file:///E:/test/baidu/audio.htm";
    "file:///E:/test/baidu/test_pdf_iframe.htm";
    "https://pacas-login.pingan.com.cn/cas/PA003/ICORE_PTS/login";
    "http://192.168.0.101:8000/index.html";
    "file:///E:/test/baidu/drag.htm";
    "http://www.w3school.com.cn/tiy/t.asp?f=hdom_prompt";
    "http://zgttest.sztaizhou.com/tz-customs-web/files/get?path=/resource/files/seal/TE/dbc26f411f4e4039afb924534447587b.png";
    "https://www.html5tricks.com/demo/html5-canvas-text-line-shine/index.html";
    "https://www.ihg.com/hotels/cn/zh/reservation";
    "file:///K:/kkMedia/blocklists/layui.htm";
    "http://www.w3school.com.cn/tiy/t.asp?f=hdom_prompt";
    "https://wwwimages2.adobe.com/www.adobe.com/content/dam/acom/cn/legal/licenses-terms/pdf/PlatformClients_PC_WWEULA-zh_CN-20150407_1357.pdf";
    "https://www.3d66.com/reshtml/6233/623334.html";
    "https://www.justeasy.cn/3d/id-350054.html";
    "https://www.om.cn/model/detail/664381";
    "file:///K:/tic-win32-ia32/close_after_print.html";
    "https://www.layui.com/demo/";
    "http://www.jq22.com/yanshi347?tdsourcetag=s_pcqq_aiomsg";
    "www.uhaozu.com";
    "about:blank";
    "https://tms.putbox.cn/login";
    "https://mozilla.github.io/pdf.js/";
    //"mbres:///dir/test_asar.htm";
    //"https://codepen.io/yangbo5207/pen/LWjWpe";
    "https://echarts.baidu.com/examples/";
    "file:///E:/mycode/mtmb/mtmb/printing/PdfViewerHostTest.htm";
    "http://v.youku.com/v_show/id_XNDAwMjkxMDM0MA==.html?spm=a2ha1.12325017.m_2540.5~1~3~A/";
    "file:///C:/cache/93527630/FileRecv/print_test.mhtml";
    "http://passport.liepin.com/v1/logout?backurl=https://passport.liepin.com/e/account";
    "https://passport.liepin.com/logout?rdurl=https://passport.liepin.com/e/account";
    "https://passport.liepin.com/logout?rdurl=https://passport.liepin.com/e/account#sfrom=click-pc_homepage-front_navigation-ecomphr_new";
    "file:///E:/test/ouchn_cn/test_print.mhtml";
    "http://192.168.0.100:8000/test_upload/test_iframe.htm";

    if (!isExist)
        return "http://miniblink.net/";
    return url;
}

void readFile(const wchar_t* path, std::vector<char>* buffer)
{
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {

        std::vector<WCHAR> filenameBuffer;
        filenameBuffer.resize(MAX_PATH + 3);
        ::GetModuleFileNameW(NULL, filenameBuffer.data(), MAX_PATH);
        ::PathRemoveFileSpecW(filenameBuffer.data());

        ::PathAppendW(filenameBuffer.data(), L"mtmb.exp");
        if (::PathFileExistsW(filenameBuffer.data())) {
#ifdef _DEBUG
            DebugBreak();
#endif
        }

        return;
    }

    DWORD fileSizeHigh;
    const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);

    DWORD numberOfBytesRead = 0;
    buffer->resize(bufferSize);
    BOOL b = ::ReadFile(hFile, &buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
    ::CloseHandle(hFile);
    b = b;
}

void writeFile(const wchar_t* path, const std::vector<char>& buffer)
{
    HANDLE hFile = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        DebugBreak();
        return;
    }

    DWORD numberOfBytesWrite = 0;
    BOOL b = ::WriteFile(hFile, &buffer.at(0), buffer.size(), &numberOfBytesWrite, nullptr);
    ::CloseHandle(hFile);
    b = b;
}

bool saveDumpFile(const std::wstring& url, const char* buffer, unsigned int size)
{
    HANDLE hFile = NULL;
    bool   bRet = false;

    hFile = CreateFileW(url.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!hFile || INVALID_HANDLE_VALUE == hFile)
        return false;

    DWORD numberOfBytesWritten = 0;
    ::WriteFile(hFile, buffer, size, &numberOfBytesWritten, NULL);
    ::CloseHandle(hFile);
    bRet = true;

    return bRet;
}

void setUserDataPtr(HWND hWnd, void* ptr)
{
    ::SetLastError(ERROR_SUCCESS);
    LONG_PTR result = ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
}

// Return the window's user data pointer.
template <typename T>
T getUserDataPtr(HWND hWnd)
{
    return reinterpret_cast<T>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

WNDPROC setWndProcPtr(HWND hWnd, WNDPROC wndProc)
{
    WNDPROC old = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(hWnd, GWLP_WNDPROC));
    LONG_PTR result = ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndProc));
    return old;
}

int RootWindow::m_globalCount = 0;
int g_quitCount = 0;

RootWindow::RootWindow()
{
    m_hWnd = nullptr;
    m_mbView = NULL_WEBVIEW;
    //m_initUrl = nullptr;
    m_font = nullptr;
    m_backHwnd = nullptr;
    m_forwardHwnd = nullptr;
    m_reloadHwnd = nullptr;
    m_stopHwnd = nullptr;
    m_editHwnd = nullptr;
    m_hbrush = nullptr;
    m_hMenu = nullptr;
    m_globalCount++;
}

RootWindow::~RootWindow()
{
    m_globalCount--;

    if (0 == m_globalCount) {
        g_quitCount = 1;
        ::PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0, 0);
    }
}

int testIeMain(HINSTANCE hInstance);

void MB_CALL_TYPE onGetMHTMLCallback(mbWebView webView, void* param, const utf8* mhtml)
{
    OutputDebugStringA("");
}

static void MB_CALL_TYPE onUrlRequestWillRedirectCallback(mbWebView webView, void* param, mbWebUrlRequestPtr oldRequest, mbWebUrlRequestPtr request, mbWebUrlResponsePtr redirectResponse)
{
}

static void MB_CALL_TYPE onUrlRequestDidReceiveResponseCallback(mbWebView webView, void* param, mbWebUrlRequestPtr request, mbWebUrlResponsePtr response)
{
}

static void MB_CALL_TYPE onUrlRequestDidReceiveDataCallback(mbWebView webView, void* param, mbWebUrlRequestPtr request, const char* data, int dataLength)
{
    OutputDebugStringA("onUrlRequestDidReceiveDataCallback\n");
}

static void MB_CALL_TYPE onUrlRequestDidFailCallback(mbWebView webView, void* param, mbWebUrlRequestPtr request, const utf8* error)
{
    OutputDebugStringA("onUrlRequestDidFailCallback\n");
}

static void MB_CALL_TYPE onUrlRequestDidFinishLoadingCallback(mbWebView webView, void* param, mbWebUrlRequestPtr request, double finishTime)
{
    OutputDebugStringA("onUrlRequestDidFinishLoadingCallback\n");
}

struct DialogSaveInfo {
    int isFinish;
    const char* data;
    size_t size;
};

static unsigned int  __stdcall dialogSaveThread(void* param)
{
    DialogSaveInfo* info = (DialogSaveInfo*)param;

    OPENFILENAMEW ofn = { 0 };
    std::vector<wchar_t> fileResult;
    fileResult.resize(4 * MAX_PATH + 1);

    wcscpy(&fileResult.at(0), L"mb_screenshot.bmp");

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = &fileResult.at(0);
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"All\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

    if (!GetSaveFileNameW(&ofn)) {
        ::MessageBoxW(nullptr, L"失败", L"打开保存对话框失败", 0);
        InterlockedIncrement((long*)&(info->isFinish));
        return 0;
    }

    HANDLE hFile = CreateFileW(&fileResult.at(0), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        ::MessageBoxW(nullptr, L"失败", L"打开文件失败", 0);
        InterlockedIncrement((long*)&(info->isFinish));
        return 0;
    }

    DWORD numberOfBytesWrite = 0;
    BOOL b = ::WriteFile(hFile, info->data, info->size, &numberOfBytesWrite, nullptr);
    ::CloseHandle(hFile);

    InterlockedIncrement((long*)&(info->isFinish));
    return 0;
}

static void MB_CALL_TYPE onPrintBitmapCallback(mbWebView webview, void* param, const char* data, size_t size)
{
    DialogSaveInfo info;
    info.isFinish = 0;
    info.data = data;
    info.size = size;

    unsigned int threadIdentifier = 0;
    HANDLE hHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, dialogSaveThread, &info, 0, &threadIdentifier));

    while (0 == info.isFinish) {
        ::Sleep(50);
    }

    ::CloseHandle(hHandle);
}

void MB_CALL_TYPE onGetCookieCallback(mbWebView webView, void* param, MbAsynRequestState state, const utf8* cookie)
{
    OutputDebugStringA("");
}

void testV8Snapshot(mbWebView mbView)
{
    //     const mbMemBuf* buf = mbUtilCreateV8Snapshot("console.log('11111')");
    //     mbFreeMemBuf((mbMemBuf*)buf);
    //mbRunJs(mbView, mbWebFrameGetMainFrame(mbView), "getRegisterInfo(){ return 1; } getRegisterInfo();", false, onRunJsCallback, nullptr, nullptr);
}

void enableDiskCache()
{
    std::vector<WCHAR> filenameBuffer;
    filenameBuffer.resize(MAX_PATH + 3);
    ::GetModuleFileNameW(NULL, filenameBuffer.data(), MAX_PATH);
    ::PathRemoveFileSpecW(filenameBuffer.data());
    ::PathAppendW(filenameBuffer.data(), L"enableDiskCache.txt");

    if (::PathFileExistsW(filenameBuffer.data())) {
        mbSetDiskCacheEnabled(NULL_WEBVIEW, FALSE);
        ::MessageBoxW(nullptr, L"关闭硬盘缓存", L"提示", 0);

        ::DeleteFileW(filenameBuffer.data());
    } else {
        mbSetDiskCacheEnabled(NULL_WEBVIEW, TRUE);
        ::MessageBoxW(nullptr, L"开启硬盘缓存", L"提示", 0);

        ::CreateFileW(filenameBuffer.data(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }
}

bool isEnableDiskCache()
{
    mbSetDiskCacheEnabled(NULL_WEBVIEW, TRUE);

    std::vector<WCHAR> filenameBuffer;
    filenameBuffer.resize(MAX_PATH + 3);
    ::GetModuleFileNameW(NULL, filenameBuffer.data(), MAX_PATH);
    ::PathRemoveFileSpecW(filenameBuffer.data());

    ::PathAppendW(filenameBuffer.data(), L"enableDiskCache.txt");

    return !!::PathFileExistsW(filenameBuffer.data());
}

void MB_CALL_TYPE onGetContentAsMarkupCallback(mbWebView webView, void* param, const utf8* content, size_t size)
{
    OutputDebugStringA("");
}

void MB_CALL_TYPE onScreenshot(mbWebView webView, void* param, const char* data, size_t size)
{
    if (0 == size)
        return;

    int* count = (int*)param;

    char path[200] = { 0 };
    sprintf_s(path, 0x99, "d:\\FeigeDownload\\%d.bmp", *count);

    (*count)++;

    HANDLE hFile = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!hFile || INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD numberOfBytesWritten = 0;
    ::WriteFile(hFile, data, size, &numberOfBytesWritten, NULL);
    ::CloseHandle(hFile);
}

static BOOL packetDirectoryImpl(LPCWSTR path, IStorage* pStg)
{
    WIN32_FIND_DATA findData;
    HANDLE hFind;

    std::wstring filePathName = path;
    filePathName += L"\\*.*";

    hFind = ::FindFirstFile(filePathName.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return FALSE;

    BOOL b = TRUE;
    while (::FindNextFile(hFind, &findData)) {
        if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0 || wcsstr(findData.cFileName, L".mbpack"))
            continue;

        std::wstring fullPathName = path;
        fullPathName += L'\\';
        fullPathName += findData.cFileName;

        HRESULT hr;
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            IStorage* pSub = nullptr;
            hr = pStg->CreateStorage(findData.cFileName, STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE, 0, 0, &pSub);
            if (!SUCCEEDED(hr)) {
                b = FALSE;
                break;
            }

            packetDirectoryImpl(fullPathName.c_str(), pSub);
            pSub->Release();
        } else {
            IStream* pStm = NULL;
            hr = pStg->CreateStream(findData.cFileName, STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE, 0, 0, &pStm);
            if (!SUCCEEDED(hr)) {
                b = FALSE;
                break;
            }

            std::vector<char> buffer;
            readFile(fullPathName.c_str(), &buffer);

            hr = pStm->Write(buffer.data(), buffer.size(), NULL);
            if (!SUCCEEDED(hr)) {
                b = FALSE;
                break;
            }
            pStm->Release();
        }
    }

    ::FindClose(hFind);
    return b;
}

BOOL packetDirectory(LPCWSTR path)
{
    std::wstring outputPath = path;
    if (outputPath.size() == 0)
        return FALSE;
    if (outputPath[outputPath.size() - 1] != L'\\')
        outputPath += L'\\';
    outputPath += L"app.mbpack";
    ::DeleteFile(outputPath.c_str());

    HRESULT hr;
    IStorage* pStg = NULL;

    hr = ::StgCreateDocfile(outputPath.c_str(), STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE, 0, &pStg);
    if (!SUCCEEDED(hr))
        return FALSE;

    BOOL b = packetDirectoryImpl(path, pStg);
    pStg->Release();
    return b;
}

void pickAndPacketDirectory()
{
    BROWSEINFO bi;
    std::vector<WCHAR> buffer;
    std::vector<WCHAR> fullPath;
    buffer.resize(MAX_PATH);
    fullPath.resize(MAX_PATH);

    bi.hwndOwner = NULL;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = buffer.data();
    bi.lpszTitle = L"Selection";
    bi.ulFlags = BIF_RETURNONLYFSDIRS;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;

    ITEMIDLIST* pidl = ::SHBrowseForFolder(&bi);
    if (!::SHGetPathFromIDList(pidl, fullPath.data()))
        return;

    if (packetDirectory(fullPath.data()))
        MessageBoxW(0, L"打包完成", L"提示", 0);
    else
        MessageBoxW(0, L"打包失败", L"提示", 0);
}

void packetAddrEditDirectory(mbWebView view, HWND hWnd)
{
    std::vector<wchar_t> strPtr;
    strPtr.resize(MAX_URL_LENGTH + 1);
    *((LPWORD)&strPtr[0]) = MAX_URL_LENGTH;
    LRESULT strLen = SendMessage(hWnd, EM_GETLINE, 0, (LPARAM)&strPtr[0]);
    if (strLen > 0) {
        strPtr[strLen] = 0;

        if (packetDirectory(strPtr.data()))
            MessageBoxW(0, L"打包完成", L"提示", 0);
        else
            MessageBoxW(0, L"打包失败", L"提示", 0);
    }
}

void MB_CALL_TYPE onPrintPdfDataCallback(mbWebView webview, void* param, const mbPdfDatas* pdfDatas)
{
    if (!pdfDatas)
        return;

    for (int i = 0; i < pdfDatas->count; ++i) {
        int size = pdfDatas->sizes[i];
        const void* data = pdfDatas->datas[i];

        saveDumpFile(L"c:\\1.pdf", (const char*)data, size);
    }
}

void printToPdf(mbWebView view)
{
    mbPrintSettings settings = { 0 };

    int dpi = 600;
    settings.width = (int)(8.266 * dpi);// dpi * 640; // in px or device context
    settings.height = (int)(11.7 * dpi);// dpi * 740;
    settings.marginLeft = 0; // marginPrinterUnits
    settings.marginTop = 0;
    settings.marginRight = 0;
    settings.marginBottom = 0;
    settings.dpi = dpi;
    settings.isPrintPageHeadAndFooter = TRUE;
    settings.isPrintBackgroud = TRUE;
    settings.isLandscape = FALSE;
    settings.isPrintToMultiPage = FALSE;

    mbUtilPrintToPdf(view, mbWebFrameGetMainFrame(view), &settings, onPrintPdfDataCallback, nullptr);
}

void testUtil(mbWebView mbView)
{
    mbRunJs(mbView, mbWebFrameGetMainFrame(mbView), "test2();", true, nullptr, nullptr, nullptr);
}

const UINT kBaseId = 100;
float g_zoomFactor = 1.0;

LRESULT RootWindow::hideWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UINT id = LOWORD(wParam);
    if (WM_COMMAND != uMsg)
        return ::DefWindowProc(hwnd, uMsg, wParam, lParam);

    if (kBaseId + 0 == id) {
        onShowDevtools();
        //searchOmSite(m_mbView);
        ///mbGetContentAsMarkup(m_mbView, onGetContentAsMarkupCallback, nullptr, (mbWebFrameHandle)-2);
    } else if (kBaseId + 1 == id) {
        RegWnd::getInst()->createRegWnd();
    } else if (kBaseId + 2 == id) {
        //mbPopupDownloadMgr(m_mbView, nullptr, nullptr);
        g_zoomFactor += 0.1f;
        if (g_zoomFactor > 1.5f)
            g_zoomFactor = 1.5f;
        mbSetZoomFactor(m_mbView, g_zoomFactor);
    } else if (kBaseId + 3 == id) {
        g_zoomFactor -= 0.1f;
        if (g_zoomFactor < 0.5f)
            g_zoomFactor = 0.5f;
        mbSetZoomFactor(m_mbView, g_zoomFactor);
    } else if (kBaseId + 4 == id) {
        mbUtilPrint(m_mbView, nullptr, nullptr);
    } else if (kBaseId + 5 == id) {
        mbSetResourceGc(m_mbView, 1);
    } else if (kBaseId + 6 == id) {
        mbUtilPrintToBitmap(m_mbView, nullptr, nullptr, onPrintBitmapCallback, nullptr);

        //int count = 0;
        //mbUtilScreenshot(m_mbView, nullptr, onScreenshot, &count);
    } else if (kBaseId + 7 == id) {
        enableDiskCache();
    } else if (kBaseId + 8 == id) {
        mbClearCookie(m_mbView);
    } else if (kBaseId + 9 == id) {
        pickAndPacketDirectory();
    } else if (kBaseId + 10 == id) {
        packetAddrEditDirectory(m_mbView, m_editHwnd);
    } else if (kBaseId + 11 == id) {
        //testUtil(m_mbView);
        mbGetCookie(m_mbView, onGetCookieCallback, nullptr);
        //printToPdf(m_mbView);
    } else if (kBaseId + 12 == id) {
        //testUtil(m_mbView);
    }

    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static void appendMenuItem(const std::wstring& labelStr, HMENU hMenu, MENUITEMINFO* info, UINT* count)
{
    info->fMask |= MIIM_STRING | MIIM_ID;
    info->cch = labelStr.size();
    info->dwTypeData = const_cast<LPWSTR>(labelStr.c_str());
    info->wID = kBaseId + *count;
    ::InsertMenuItem(hMenu, *count, TRUE, info);
    (*count)++;
}

void RootWindow::onMoreCommand()
{
    POINT pt = { 0 };
    ::GetCursorPos(&pt);
    if (m_hMenu) {
        ::SetForegroundWindow(m_hideWndHelp->getWnd());
        ::SetFocus(m_hideWndHelp->getWnd());
        ::TrackPopupMenu(m_hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hideWndHelp->getWnd(), NULL);
        return;
    }

    RootWindow* self = this;
    m_hideWndHelp = new HideWndHelp(L"HideParentWindowClass", [self](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        return self->hideWndProc(hWnd, uMsg, wParam, lParam);
    });

    m_hMenu = ::CreatePopupMenu();

    MENUITEMINFO info = { 0 };
    info.cbSize = sizeof(MENUITEMINFO);
    UINT count = 0;
    appendMenuItem(L"打开devtools", m_hMenu, &info, &count);
    appendMenuItem(L"申请注册版", m_hMenu, &info, &count);
    appendMenuItem(L"放大", m_hMenu, &info, &count);
    appendMenuItem(L"缩小", m_hMenu, &info, &count);
    appendMenuItem(L"打印测试", m_hMenu, &info, &count);
    appendMenuItem(L"内存回收GC", m_hMenu, &info, &count);
    appendMenuItem(L"截图", m_hMenu, &info, &count);
    //appendMenuItem(L"创建js快照", m_hMenu, &info, &count);
    appendMenuItem(L"打开硬盘缓存", m_hMenu, &info, &count);
    appendMenuItem(L"清除Cookies", m_hMenu, &info, &count);
    appendMenuItem(L"选择文件夹并打包资源", m_hMenu, &info, &count);
    appendMenuItem(L"地址栏打包资源资源", m_hMenu, &info, &count);
    //appendMenuItem(L"打印成pdf", m_hMenu, &info, &count);
    appendMenuItem(L"杂项测试", m_hMenu, &info, &count);

    ::SetForegroundWindow(m_hideWndHelp->getWnd());
    ::SetFocus(m_hideWndHelp->getWnd());
    ::TrackPopupMenu(m_hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hideWndHelp->getWnd(), NULL);

    return;
}

void RootWindow::onShowDevtools()
{
    //mbUtilSerializeToMHTML(m_mbView, onGetMHTMLCallback, nullptr);

    //mbRunJs(m_mbView, (mbWebFrameHandle)2, "document.documentElement.outerHTML;", true, onRunJsCallback, nullptr, nullptr);

    //     mbWebUrlRequestPtr request = mbNetCreateWebUrlRequest("https://files-cdn.cnblogs.com/files/xt_hjl/20160922001.zip", "GET", "application/zip");
    // 
    //     mbUrlRequestCallbacks callbacks = {
    //         onUrlRequestWillRedirectCallback,
    //         onUrlRequestDidReceiveResponseCallback,
    //         onUrlRequestDidReceiveDataCallback,
    //         onUrlRequestDidFailCallback,
    //         onUrlRequestDidFinishLoadingCallback
    //     };
    //     int netRequestId = mbNetStartUrlRequest(m_mbView, request, this, &callbacks);
    //     return;

    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    memset(&path[0], 0, sizeof(wchar_t) * (MAX_PATH + 1));
    ::GetModuleFileNameW(nullptr, &path[0], MAX_PATH);
    ::PathRemoveFileSpecW(&path[0]);

    std::vector<wchar_t> pathXX = path;
    ::PathAppendW(&path[0], L"front_end\\inspector.html");

    std::string pathA = "g:/mycode/mb/third_party/WebKit/Source/devtools/front_end/inspector.html";
    if (::PathFileExistsA(pathA.c_str())) {
        mbSetDebugConfig(m_mbView, "showDevTools", pathA.c_str());
        return;
    }

    if (::PathFileExistsW(&path[0])) {
        pathA = utf16ToUtf8(&path[0]);
        mbSetDebugConfig(m_mbView, "showDevTools", pathA.c_str());
    }
}

//////////////////////////////////////////////////////////////////////////

bool RootWindow::onCommand(UINT id)
{
    switch (id) {
    case IDC_NAV_BACK:   // Back button
        mbGoBack(m_mbView);
        return true;
    case IDC_NAV_FORWARD:  // Forward button
        mbGoForward(m_mbView);
        return true;
    case IDC_NAV_RELOAD:  // Reload button
        mbReload(m_mbView);
        return true;
    case IDC_NAV_STOP:  // Stop button
        mbStopLoading(m_mbView);
        return true;
    case IDC_NAV_TEST:
        onMoreCommand();

        //////////////////////////////////////////////////////////////////////////
        //         mbRunJsByFrame(m_mbView, mbWebFrameGetMainFrame(m_mbView),
        //             "var htmlObj = document.getElementById(\"ne_wrap\");\n"
        //             "var bodyObj =document.getElementsByTagName('body');\n"
        //             "var computedStyle = document.defaultView.getComputedStyle(htmlObj, null);\n"
        //             "console.log('htmlObj:' + computedStyle.overflowY);\n"
        //             "computedStyle = document.defaultView.getComputedStyle(bodyObj[0], null);\n"
        //             "console.log('bodyObj:' + computedStyle.overflowY);\n"
        //             , true);
        // 
        //         static int count = 0;
        //         if ((count % 2) == 0) {
        //             mbInsertCSSByFrame(m_mbView, mbWebFrameGetMainFrame(m_mbView), "html { overflow:hidden; }");
        //             OutputDebugStringA("hidden! \n");
        //         } else {
        //             mbInsertCSSByFrame(m_mbView, mbWebFrameGetMainFrame(m_mbView), "html { overflow:visible; }");
        //             OutputDebugStringA("visible! \n");
        //         }
        //         count++;
        //////////////////////////////////////////////////////////////////////////
        return true;
    }

    return false;
}

LRESULT CALLBACK rootWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void registerRootClass(HINSTANCE hInstance, const std::wstring& window_class, HBRUSH background_brush)
{
    // Only register the class one time.
    static bool classRegistered = false;
    if (classRegistered)
        return;
    classRegistered = true;

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = rootWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr; // LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WKEXE));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = background_brush;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = window_class.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassEx(&wcex);
}

void RootWindow::onCanGoForwardCallback(mbWebView webView, void* param, MbAsynRequestState state, BOOL b)
{
    if (kMbAsynRequestStateOk != state)
        return;
    RootWindow* self = (RootWindow*)param;
    ::EnableWindow(self->m_forwardHwnd, b);
}

void RootWindow::onCanGoBackCallback(mbWebView webView, void* param, MbAsynRequestState state, BOOL b)
{
    if (kMbAsynRequestStateOk != state)
        return;
    RootWindow* self = (RootWindow*)param;
    ::EnableWindow(self->m_backHwnd, b);
}

void RootWindow::onUrlChangedCallback(mbWebView webView, void* param, const utf8* url, BOOL canGoBack, BOOL canGoForward)
{
    RootWindow* self = (RootWindow*)param;
    std::wstring urlString = utf8ToUtf16(url);
    ::SetWindowText(self->m_editHwnd, urlString.c_str());

    //     mbCanGoForward(self->m_mbView, onCanGoForwardCallback, self);
    //     mbCanGoBack(self->m_mbView, onCanGoBackCallback, self);
    ::EnableWindow(self->m_backHwnd, canGoBack);
    ::EnableWindow(self->m_forwardHwnd, canGoForward);
}

static BOOL MB_CALL_TYPE handleLoadUrlBegin(mbWebView webView, void* param, const char* url, void* job);
static void MB_CALL_TYPE handleLoadUrlEnd(mbWebView webView, void* param, const char* url, void* job, void* buf, int len);

void MB_CALL_TYPE onRunJsCallback(mbWebView webView, void* param, mbJsExecState es, mbJsValue v)
{
    double vv = mbJsToDouble(es, v);
    OutputDebugStringA("");
}

void RootWindow::onDocumentReady(mbWebView webView, void* param, mbWebFrameHandle frameId)
{
    if (mbIsMainFrame(webView, frameId)) {
        OutputDebugStringA("HandleDocumentReady main----------------\n");
    } else
        OutputDebugStringA("HandleDocumentReady not main------------------\n");

    //     mbRunJs(webView, mbWebFrameGetMainFrame(webView), 
    //         "for(var i=0;i<2;i++){\n"
    //         "    var abc = document.getElementById('content');\n"
    //         "    abc.innerHTML = abc.innerHTML + '123\\n';\n"
    //         "} ;2;", false, onRunJsCallback, NULL, NULL);
}

void RootWindow::onLoadingFinish(mbWebView webView, void* param, mbWebFrameHandle frameId, const utf8* url, mbLoadingResult result, const utf8* failedReason)
{

    //    mbRunJs(webView, mbWebFrameGetMainFrame(webView), "window.location.reload();", true, nullptr, nullptr, nullptr);
    //     if (mbIsMainFrame(webView, frameId))
    //         OutputDebugStringA("handleLoadingFinish main=======================\n");
    //     else
    //         OutputDebugStringA("handleLoadingFinish not main======================\n");
}

static BOOL* isRegistered = nullptr;

static void setTitle(HWND hWnd, const std::wstring& title)
{
    if (!isRegistered) {
        isRegistered = new BOOL();
        *isRegistered = mbUtilIsRegistered(L"");
    }

    std::wstring titleString = title;
    if (*isRegistered)
        titleString = L"(VIP已注册) " + titleString;
    else
        titleString = L"(VIP未注册) " + titleString;

    if (titleString.size() > 65) {
        titleString = titleString.substr(0, 65);
        titleString += L"...";
    }

    ::SetWindowText(hWnd, titleString.c_str());
}

static std::wstring getVersionString()
{
    static std::wstring* s_title = nullptr;
    if (s_title)
        return *s_title;

    std::vector<wchar_t> versionPath;
    versionPath.resize(MAX_PATH);
    ::GetModuleFileNameW(nullptr, &versionPath[0], MAX_PATH);
    ::PathRemoveFileSpecW(&versionPath[0]);
#ifdef _WIN64
    ::PathAppend(&versionPath[0], L"mb_x64.dll");
#else
    ::PathAppend(&versionPath[0], L"mb.dll");
#endif

    wchar_t versionCurrent[35];
    DWORD fileVersionSize = ::GetFileVersionInfoSizeW(&versionPath[0], 0);
    wchar_t* lpData = new wchar_t[fileVersionSize];
    if (!::GetFileVersionInfoW(&versionPath[0], NULL, fileVersionSize, lpData)) {
        delete lpData;
        return L"";
    }

    VS_FIXEDFILEINFO* versionInfo;
    PUINT uint = 0;
    if (!VerQueryValueW(lpData, L"\\", (LPVOID*)&versionInfo, uint)) {
        delete lpData;
        return L"";
    }
    wnsprintf(versionCurrent, 35, L" [%d.%d.%d.%d] ",
        HIWORD(versionInfo->dwProductVersionMS),
        LOWORD(versionInfo->dwProductVersionMS),
        HIWORD(versionInfo->dwProductVersionLS),
        LOWORD(versionInfo->dwProductVersionLS));

    s_title = new std::wstring(versionCurrent);
    return *s_title;
}

static void MB_CALL_TYPE onTitleChangedCallback(mbWebView webView, void* param, const utf8* title)
{
    RootWindow* self = (RootWindow*)param;
    std::wstring titleString = utf8ToUtf16(title);

    titleString = getVersionString() + titleString;

    setTitle(self->getHwnd(), titleString);
}

struct DownInfo {
    std::string url;
    size_t recvSize;
    size_t allSize;
};

void MB_CALL_TYPE onNetJobDataRecvCallback(void* ptr, mbNetJob job, const char* data, int length)
{
    DownInfo* info = (DownInfo*)ptr;
    info->recvSize += length;

    char* output = (char*)malloc(0x1000);
    sprintf_s(output, 0x990, "onNetJobDataRecvCallback: %d %d, %f\n", info->allSize, info->recvSize, info->recvSize / (info->allSize + 1.0));
    OutputDebugStringA(output);
    free(output);
}

unsigned int __stdcall msgBoxThread(void* param)
{
    std::function<void(void)>* callback = (std::function<void(void)>*)param;
    (*callback)();
    delete callback;
    return 0;
}

void MB_CALL_TYPE onNetJobDataFinishCallback(void* ptr, mbNetJob job, mbLoadingResult result)
{
    OutputDebugStringA("onNetJobDataFinishCallback\n");

    DownInfo* info = (DownInfo*)ptr;
    std::string url = info->url;

    wchar_t temp[20] = { 0 };
    wsprintf(temp, L"%d", info->recvSize);

    std::wstring* title = new std::wstring(utf8ToUtf16(url));
    *title += L" 下载完成：";
    *title += temp;

    delete info;

    std::function<void(void)>* callback = new std::function<void(void)>([title, result] {
        LPCWSTR lpCaption = (result == MB_LOADING_SUCCEEDED ? L"下载成功" : L"下载失败");
        MessageBoxW(nullptr, title->c_str(), lpCaption, MB_OK);
        delete title;
    });

    unsigned int threadId = 0; // 为了不卡blink线程，messagbox放到另外个线程弹出
    HANDLE threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, msgBoxThread, callback, 0, &threadId));
    ::CloseHandle(threadHandle);
}

static mbDownloadOpt MB_CALL_TYPE onDownloadCallback(mbWebView webView,
    void* param,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    mbNetJob job,
    mbNetJobDataBind* dataBind)
{
    char* output = (char*)malloc(0x8000);
    sprintf_s(output, 0x7990, "onDownloadCallback: %d %s\n", expectedContentLength, url);
    OutputDebugStringA(output);
    free(output);

    DownInfo* info = new DownInfo();
    info->url = url;
    info->recvSize = 0;
    info->allSize = expectedContentLength;

    mbDownloadBind bind = { 0 };
    bind.param = info;
    bind.recvCallback = onNetJobDataRecvCallback;
    bind.finishCallback = onNetJobDataFinishCallback;
    bind.saveNameCallback = nullptr;

    return mbPopupDialogAndDownload(webView, param, expectedContentLength, url, mime, disposition, job, dataBind, &bind);
    //return mbDownloadByPath(webView, param, L"P:\\", expectedContentLength, url, mime, disposition, job, dataBind, &bind);
}

mbStringPtr MB_CALL_TYPE onPromptBoxCallback(mbWebView webView, void* param, const utf8* msg, const utf8* defaultResult)
{
    return mbCreateString("123", 3);
}

void MB_CALL_TYPE onJsQueryCallback(mbWebView webView, void* param, mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)
{
    OutputDebugStringA("");
}

BOOL MB_CALL_TYPE onNavigation(mbWebView webView, void* param, mbNavigationType navigationType, const utf8* url)
{
    OutputDebugStringA(url);
    OutputDebugStringA("\n");
    return TRUE;
}

void MB_CALL_TYPE onDidCreateScriptContextCallback(mbWebView webView, void* param, mbWebFrameHandle frameId, void* context, int extensionGroup, int worldId)
{
    //mbRunJs(webView, mbWebFrameGetMainFrame(webView), s_injectScript, FALSE, nullptr, nullptr, nullptr);
    //OutputDebugStringA("");
}

BOOL MB_CALL_TYPE onGetPluginListCallback(BOOL refresh, void* builder, void* param)
{
    mbPluginListBuilderAddPlugin(builder, "Tencent QQ", "dll", "npactivex.dll");
    mbPluginListBuilderAddMediaTypeToLastPlugin(builder, "application/qscall-plugin", "dll");
    mbPluginListBuilderAddFileExtensionToLastMediaType(builder, "dll");

    mbPluginListBuilderAddPlugin(builder, "XunLei Plugin", "Xunlei scriptability Plugin", "npxunlei1.0.0.2.dll");
    mbPluginListBuilderAddMediaTypeToLastPlugin(builder, "application/np_xunlei_plugin", "np_xunlei_plugin");
    mbPluginListBuilderAddFileExtensionToLastMediaType(builder, "dll");

    return FALSE;
}

mbStringPtr MB_CALL_TYPE onWillConnect(mbWebView webView, void* param, mbWebSocketChannel channel, const utf8* url, BOOL* needHook)
{
    *needHook = TRUE; // 如果是需要拦截的url，就设置这个为TRUE，否则FALSE
    const char* newUrl = "http://baidu.com/"; // 如果需要拦截url，就写上。如果不需要就返回nullptr
    return mbCreateString(newUrl, strlen(newUrl));
}

BOOL MB_CALL_TYPE onConnected(mbWebView webView, void* param, mbWebSocketChannel channel)
{
    return false;
}

mbStringPtr MB_CALL_TYPE onReceive(mbWebView webView, void* param, mbWebSocketChannel channel, int opCode, const char* buf, size_t len, BOOL* isContinue)
{
    *isContinue = true;
    return nullptr;
}

mbStringPtr MB_CALL_TYPE onSend(mbWebView webView, void* param, mbWebSocketChannel channel, int opCode, const char* buf, size_t len, BOOL* isContinue)
{
    *isContinue = true;
    return nullptr; // 如果不需要替换发送数据，就返回nullptr
    return mbCreateStringWithoutNullTermination("123", 3); // 如果需要替换发送数据就在这里返回
}

void MB_CALL_TYPE onError(mbWebView webView, void* param, mbWebSocketChannel channel)
{

}

mbStringPtr MB_CALL_TYPE onImageBufferToDataURLCallback(mbWebView webView, void* param, const char* data, size_t size)
{
    return mbCreateString(data, size);
}

void MB_CALL_TYPE onPaintBitUpdatedCallback(mbWebView webView, void* param, const void* buffer, const mbRect* r, int width, int height)
{
    //     int size = width * height;
    //     int whiteSize = 0;
    //     for (int y = 0; y < height; y++) {
    //         for (int x = 0; x < width; x++) {
    //             const unsigned int* ptr = (const unsigned int*)buffer;
    //             const unsigned int* ptrDst = &((ptr)[x + y * width]);
    //             int c = *ptrDst;
    //             if (c & 0xff000000) {
    //                 whiteSize++;
    //             }
    //         }
    //     }
    //     if (size == whiteSize)
    //         OutputDebugStringA("onPaintBitUpdatedCallback fail\n");
    //     else
    //         OutputDebugStringA("onPaintBitUpdatedCallback ok\n");
}

mbWebView RootWindow::onCreateView(mbWebView parentWebviwe, void* param, mbNavigationType navType, const utf8* url, const mbWindowFeatures* features)
{
    RootWindow* rootWin = new RootWindow();
    mbWebView newWindow = rootWin->createRootWindow();

    std::wstring urlString = utf8ToUtf16(url);
    ::SetWindowText(rootWin->m_editHwnd, urlString.c_str());

    //mbSetCookieJarFullPath(newWindow, L"G:\\test\\exe_test\\publicdll\\2.dat");
    //mbSetLocalStorageFullPath(newWindow, L"d:\\我Storage\\");

    mbShowWindow(newWindow, true);
    return newWindow;
}

static bool hookUrl(void* job, const char* url, const char* hookUrl, const wchar_t* localFile, const char* mime)
{
    if (0 == strstr(url, hookUrl))
        return false;

    std::vector<char> buffer;
    readFile(localFile, &buffer);
    if (0 == buffer.size())
        return false;

    mbNetSetData(job, &buffer[0], buffer.size());

    OutputDebugStringA("hookUrl:");
    OutputDebugStringA(url);
    OutputDebugStringA("\n");

    return true;
}

static bool hookUrl2(void* job, const char* url, const char* hookUrl, const wchar_t* localFile, const char* mime)
{
    if (0 != strcmp(url, hookUrl))
        return false;

    std::vector<char> buffer;
    readFile(localFile, &buffer);
    if (0 == buffer.size())
        return false;

    mbNetSetData(job, &buffer[0], buffer.size());

    OutputDebugStringA("hookUrl:");
    OutputDebugStringA(url);
    OutputDebugStringA("\n");

    return true;
}

void* wkeCreateResPacketMgr(const char* data, unsigned int len);
bool wkeOnUrlBeginResPacketMgr(void* mrg, const char* url, void* job);

void RootWindow::onPaint()
{
    //     if (WS_EX_LAYERED == (WS_EX_LAYERED & ::GetWindowLong(m_hWnd, GWL_EXSTYLE)))
    //         return;
    // 
    //     PAINTSTRUCT ps = { 0 };
    //     HDC hdc = ::BeginPaint(m_hWnd, &ps);
    // 
    //     RECT rcClip = ps.rcPaint;
    // 
    //     RECT rcClient;
    //     ::GetClientRect(m_hWnd, &rcClient);
    // 
    //     RECT rcInvalid = rcClient;
    //     if (rcClip.right != rcClip.left && rcClip.bottom != rcClip.top)
    //         ::IntersectRect(&rcInvalid, &rcClip, &rcClient);
    // 
    //     int srcX = rcInvalid.left - rcClient.left;
    //     int srcY = rcInvalid.top - rcClient.top;
    //     int destX = rcInvalid.left;
    //     int destY = rcInvalid.top;
    //     int width = rcInvalid.right - rcInvalid.left;
    //     int height = rcInvalid.bottom - rcInvalid.top;
    // 
    //     if (0 != width && 0 != height) {
    //         HDC hMbDC = mbGetLockedViewDC(m_mbView);
    //         ::BitBlt(hdc, destX, destY, width, height, hMbDC, srcX, srcY, SRCCOPY);
    //         mbUnlockViewDC(m_mbView);
    //     }
    // 
    //     ::EndPaint(m_hWnd, &ps);
}

void RootWindow::onSize(bool minimized)
{
    RECT rect;
    ::GetClientRect(m_hWnd, &rect);
    // Resize the window and address bar to match the new frame size.
    rect.top += urlbarHeight;

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int urloffset = rect.left + buttonWidth * 5;

    // Resize just the URL edit field.
    ::SetWindowPos(m_editHwnd, NULL, urloffset, 0, rect.right - urloffset, urlbarHeight, SWP_NOZORDER);
    mbResize(m_mbView, width, height);
    mbWake(m_mbView);
}

void RootWindow::onFocus()
{
    //     HWND hWnd = mbGetHostHWND(m_mbView);
    //     mbSetFocus(m_mbView);
    //     ::SetCapture(hWnd);
}

void RootWindow::onWheel(WPARAM wParam, LPARAM lParam)
{
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);

    unsigned int flags = 0;

    if (wParam & MK_CONTROL)
        flags |= MB_CONTROL;
    if (wParam & MK_SHIFT)
        flags |= MB_SHIFT;

    if (wParam & MK_LBUTTON)
        flags |= MB_LBUTTON;
    if (wParam & MK_MBUTTON)
        flags |= MB_MBUTTON;
    if (wParam & MK_RBUTTON)
        flags |= MB_RBUTTON;

    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    mbFireMouseWheelEvent(m_mbView, x, y, delta, flags);
}

void RootWindow::onDestroyed()
{
    setWndProcPtr(m_hWnd, nullptr);
    delete this;
}

void RootWindow::onTimer()
{
    //mbReload(m_mbView);
}

LRESULT CALLBACK rootWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RootWindow* self = getUserDataPtr<RootWindow*>(hWnd);
    if (!self)
        return ::DefWindowProc(hWnd, message, wParam, lParam);

    // Callback for the main window
    switch (message) {
    case WM_COMMAND:
        if (self->onCommand(LOWORD(wParam)))
            return 0;
        break;

    case WM_PAINT:
        self->onPaint();
        break;

    case WM_SIZE:
        self->onSize(wParam == SIZE_MINIMIZED);
        break;

    case WM_SETFOCUS:
        self->onFocus();
        return 0;

    case WM_KILLFOCUS:
        //mbKillFocus(self->getWkeView());
        return 0;

    case WM_MOUSEWHEEL:
        self->onWheel(wParam, lParam);
        break;
        // 
        //     case WM_MOVING:
        //     case WM_MOVE:
        //         self->OnMove();
        //         return 0;
        // 
    case WM_ERASEBKGND:
        //         if (self->OnEraseBkgnd())
        //             break;
        // Don't erase the background.
        break;

    case WM_TIMER:
        self->onTimer();
        break;
        // 
        //     case WM_ENTERMENULOOP:
        //         if (!wParam) {
        //             // Entering the menu loop for the application menu.
        //             CefSetOSModalLoop(true);
        //         }
        //         break;
        // 
        //     case WM_EXITMENULOOP:
        //         if (!wParam) {
        //             // Exiting the menu loop for the application menu.
        //             CefSetOSModalLoop(false);
        //         }
        //         break;
        // 
        //     case WM_CLOSE:
        //         if (self->OnClose())
        //             return 0;  // Cancel the close.
        //         break;
        // 
        //     case WM_NCHITTEST:
        //     {
        //         LRESULT hit = DefWindowProc(hWnd, message, wParam, lParam);
        //         if (hit == HTCLIENT) {
        //             POINTS points = MAKEPOINTS(lParam);
        //             POINT point = { points.x, points.y };
        //             ::ScreenToClient(hWnd, &point);
        //             if (::PtInRegion(self->draggable_region_, point.x, point.y)) {
        //                 // If cursor is inside a draggable region return HTCAPTION to allow
        //                 // dragging.
        //                 return HTCAPTION;
        //             }
        //         }
        //         return hit;
        //     }
        // 
    case WM_NCDESTROY:
        // Clear the reference to |self|.
        //SetUserDataPtr(hWnd, NULL);
        self->onDestroyed();
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

WNDPROC editWndProcOld = nullptr;

LRESULT CALLBACK RootWindow::editWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CHAR:
        if (wParam == VK_RETURN) {
            HWND parent = GetParent(hWnd);
            RootWindow* self = getUserDataPtr<RootWindow*>(parent);

            // When the user hits the enter key load the URL.
            std::vector<wchar_t> strPtr;
            strPtr.resize(MAX_URL_LENGTH + 1);
            *((LPWORD)&strPtr[0]) = MAX_URL_LENGTH;
            LRESULT strLen = SendMessage(hWnd, EM_GETLINE, 0, (LPARAM)&strPtr[0]);
            if (strLen > 0) {
                strPtr[strLen] = 0;

                std::string url = utf16ToUtf8(&strPtr[0]);
                mbLoadURL(self->m_mbView, url.c_str());
            }

            return 0;
        }
        if (wParam == 1 && 0x1e0001 == lParam) {
            ::PostMessage(hWnd, EM_SETSEL, 0, -1);
        }
        break;
    case WM_NCDESTROY:
        // Clear the reference to |self|.
        break;
    }

    return CallWindowProc(editWndProcOld, hWnd, message, wParam, lParam);
}

mbWebView RootWindow::createRootWindow()
{
    RECT startRect;
    startRect.left = 400;
    startRect.top = 200;
    startRect.right = startRect.left + 1080;
    startRect.bottom = startRect.top + 680;
    const DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;

    const std::wstring& window_title = L"Miniblink";
    const std::wstring& windowClass = L"MBRootWindow";

    HINSTANCE hInstance = GetModuleHandle(NULL);
    registerRootClass(hInstance, windowClass, nullptr);

    int x, y, width, height;
    if (::IsRectEmpty(&startRect)) {
        // Use the default window position/size.
        x = y = width = height = CW_USEDEFAULT;
    } else {
        // Adjust the window size to account for window frame and controls.
        RECT window_rect = startRect;
        ::AdjustWindowRectEx(&window_rect, dwStyle, false, 0);

        x = startRect.left;
        y = startRect.top;
        width = window_rect.right - window_rect.left;
        height = window_rect.bottom - window_rect.top;
    }

    // Create the main window initially hidden.
    m_hWnd = CreateWindow(windowClass.c_str(), window_title.c_str(),
        dwStyle,
        x, y, width, height,
        NULL, NULL, hInstance, NULL);
    //::SetTimer(m_hWnd, (UINT_PTR)m_hWnd, 2000, nullptr);

    setUserDataPtr(m_hWnd, this);

    createChildControl(m_hWnd);

    mbWebView mbView = mbCreateWebWindow(MB_WINDOW_TYPE_CONTROL, m_hWnd, 0, urlbarHeight, width - 8, height - urlbarHeight - 20);
    setMBView(mbView);

    //mbSetCookie(mbView, "youku.com", "cna=4UvTFE12fEECAXFKf4SFW5eo; expires=Tue, 23-Jan-29 13:17:21 GMT; path=/; domain=.youku.com");
    initSettings();

    static bool g_firstCall = true;
    if (g_firstCall) {
        std::string url = getUrl();
        mbLoadURL(mbView, url.c_str());
    }
    g_firstCall = false;

    setTitle(m_hWnd, L"");

    mbShowWindow(mbView, TRUE);
    ::ShowWindow(m_hWnd, SW_SHOW);
    ::UpdateWindow(m_hWnd);

    m_mbView = mbView;
    return mbView;
}

void RootWindow::createChildControl(HWND parentHwnd)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Create the child controls.
    int xOffset = 0;

    RECT rect;
    ::GetClientRect(parentHwnd, &rect);

    // Create a scaled font.
    m_font = ::CreateFont(
        -fontHeight, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

    m_backHwnd = ::CreateWindow(
        L"BUTTON", L"Back",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
        xOffset, 0, buttonWidth, urlbarHeight,
        parentHwnd, reinterpret_cast<HMENU>(IDC_NAV_BACK), hInstance, 0);
    ::SendMessage(m_backHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
    xOffset += buttonWidth;

    m_forwardHwnd = ::CreateWindow(
        L"BUTTON", L"Forward",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
        xOffset, 0, buttonWidth, urlbarHeight,
        parentHwnd, reinterpret_cast<HMENU>(IDC_NAV_FORWARD), hInstance, 0);
    ::SendMessage(m_forwardHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
    xOffset += buttonWidth;

    m_reloadHwnd = ::CreateWindow(
        L"BUTTON", L"Reload",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON /*| WS_DISABLED*/,
        xOffset, 0, buttonWidth, urlbarHeight,
        parentHwnd, reinterpret_cast<HMENU>(IDC_NAV_RELOAD), hInstance, 0);
    ::SendMessage(m_reloadHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
    xOffset += buttonWidth;

    m_stopHwnd = ::CreateWindow(
        L"BUTTON", L"Stop",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
        xOffset, 0, buttonWidth, urlbarHeight,
        parentHwnd, reinterpret_cast<HMENU>(IDC_NAV_STOP), hInstance, 0);
    ::SendMessage(m_stopHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
    xOffset += buttonWidth;

    m_testHwnd = ::CreateWindow(
        L"BUTTON", L"更多",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        xOffset, 0, buttonWidth, urlbarHeight,
        parentHwnd, reinterpret_cast<HMENU>(IDC_NAV_TEST), hInstance, 0);
    ::SendMessage(m_testHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
    xOffset += buttonWidth;

    m_editHwnd = ::CreateWindow(
        L"EDIT", 0,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOVSCROLL |
        ES_AUTOHSCROLL /*| WS_DISABLED*/,
        xOffset, 0, rect.right - buttonWidth * 5, urlbarHeight,
        parentHwnd, 0, hInstance, 0);
    ::SendMessage(m_editHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);

    // Override the edit control's window procedure.
    editWndProcOld = setWndProcPtr(m_editHwnd, editWndProc);
}

void RootWindow::initSettings()
{
    //     mbOnPluginList(m_mbView, onGetPluginListCallback, nullptr);
    //     mbSetDeviceParameter(m_mbView, "navigator.deviceMemory", nullptr, 8, 0);
    //     mbSetDeviceParameter(m_mbView, "navigator.connection.type", nullptr, 3, 0); // 3表示wifi

#if 0
    mbProxy proxy = { MB_PROXY_HTTP , "127.0.0.1", 1080 };
    mbSetProxy(nullptr, &proxy);
#endif
    mbSetTransparent(m_mbView, false);

    mbSetDragDropEnable(m_mbView, true);

    //if (isEnableDiskCache())
        mbSetDiskCacheEnabled(NULL_WEBVIEW, TRUE);

    //mbSetLocalStorageFullPath(nullptr, L"d:\\我Storage\\");
    //mbSetCookieJarFullPath(nullptr, L"d:\\我Storage\\init_cookie.txt");

    mbSetDebugConfig(m_mbView, "wakeMinInterval", "5"); // ?????1-200
    mbSetDebugConfig(m_mbView, "drawMinInterval", "300");
    mbSetDebugConfig(m_mbView, "contentScale", "100");
    //mbSetDebugConfig(m_mbView, "enableNodejs", "1");
    //mbSetDebugConfig(m_mbView, "drawTileLine", "true");
    //mbSetDebugConfig(m_mbView, "disableNavigatorPlugins", "true");
    mbSetCookieEnabled(m_mbView, true);
    mbSetNavigationToNewWindowEnable(m_mbView, true);
    mbSetCspCheckEnable(m_mbView, true);
    mbSetUserAgent(m_mbView, "UserAgent Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like XNT) Chrome/79.0.3945.130 Safari/537.36");

    //mbNetEnableResPacket(m_mbView, L"E:\\mycode\\mbvip\\out\\x86\\Debug\\1.mbpack");

    //mbSetZoomFactor(m_mbView, 1.5f);

    //     static int count = 1;
    //     wchar_t* cookiesPath = (wchar_t*)malloc(0x100);
    //     wsprintf(cookiesPath, L"G:\\test\\exe_test\\publicdll\\%d.dat", count++);
    //     mbSetCookieJarFullPath(m_mbView, cookiesPath);
    //     free(cookiesPath);

    mbOnImageBufferToDataURL(m_mbView, onImageBufferToDataURLCallback, nullptr);

    mbSetDragDropEnable(m_mbView, TRUE);
    mbSetHeadlessEnabled(m_mbView, FALSE);
    mbSetTouchEnabled(m_mbView, FALSE);
    mbSetMouseEnabled(m_mbView, TRUE);
    mbSetResourceGc(m_mbView, 30);
    mbOnURLChanged(m_mbView, onUrlChangedCallback, this);
    mbOnCreateView(m_mbView, onCreateView, this);

    mbOnLoadUrlBegin(m_mbView, handleLoadUrlBegin, nullptr);
    mbOnLoadUrlEnd(m_mbView, handleLoadUrlEnd, nullptr);

    mbOnNavigation(m_mbView, onNavigation, nullptr);
    mbOnDocumentReady(m_mbView, onDocumentReady, nullptr);
    mbOnLoadingFinish(m_mbView, onLoadingFinish, nullptr);
    mbOnDownloadInBlinkThread(m_mbView, onDownloadCallback, nullptr);
    mbOnTitleChanged(m_mbView, onTitleChangedCallback, this);
    mbOnJsQuery(m_mbView, onJsQueryCallback, nullptr);

    mbOnPaintBitUpdated(m_mbView, onPaintBitUpdatedCallback, nullptr);

    //mbSetDebugConfig(m_mbView, "setPrintingEdgeDistance", "20,10,0,0");

    mbWebsocketHookCallbacks websocketCallback;
    websocketCallback.onWillConnect = onWillConnect;
    websocketCallback.onConnected = onConnected;
    websocketCallback.onReceive = onReceive;
    websocketCallback.onSend = onSend;
    websocketCallback.onError = onError;
    //mbNetSetWebsocketCallback(m_mbView, &websocketCallback, nullptr);

    //mbOnPromptBox(m_mbView, onPromptBoxCallback, nullptr);
    //mbSetContextMenuItemShow(m_mbView, kMbMenuPrintId, TRUE);
    //mbSetDebugConfig(nullptr, "setStackLimit", "505856");
    mbSetContextMenuItemShow(m_mbView, kMbMenuPrintId, true);
    mbOnDidCreateScriptContext(m_mbView, onDidCreateScriptContextCallback, nullptr);
}

static BOOL MB_CALL_TYPE handleLoadUrlBegin(mbWebView webView, void* param, const char* url, void* job)
{
//     OutputDebugStringA("handleLoadUrlBegin:");
//     OutputDebugStringA(url);
//     OutputDebugStringA("\n");

    //mbNetSetHTTPHeaderField(job, L"referer", L"11111111111111111", false);

    if (hookUrl(job, url, "umi.876afd07.js", L"G:\\test\\web_test\\tigerkin\\umi.876afd07.js", "text/javascript"))
        return true;
    if (hookUrl(job, url, "vendors.507312f5.async.js", L"G:\\test\\web_test\\tigerkin\\vendors.507312f5.async.js", "text/javascript"))
        return true;
// 
//     if (hookUrl(job, url, "act/webcc/link-pk-game/v1.9.1/index.js", L"D:\\test\\web\\cc_163\\webcc_191_index.js", "text/javascript"))
//         return true;
// 
//     if (hookUrl(job, url, "act/webcc/performance-reporter/v1.2.0/index.js", L"D:\\test\\web\\cc_163\\performance-reporter.js", "text/javascript"))
//         return true;
// 
//     if (hookUrl(job, url, "act/webcc/h5player/v0.39.17/index.js", L"D:\\test\\web\\cc_163\\h5player.js", "text/javascript"))
//         return true;
// 
//     if (hookUrl(job, url, "act/webcc/luxury-car-sdk/v1.3.1/index.js", L"D:\\test\\web\\cc_163\\luxury-car-sdk.js", "text/javascript"))
//         return true;
    // 
    //     if (hookUrl(job, url, "h5_live_index.js", L"G:\\test\\web_test\\cctv\\h5_live_index.js", "text/javascript"))
    //         return true;
    // 
    //     if (hookUrl(job, url, "html5player_analysis_lib.js", L"G:\\test\\web_test\\cctv\\html5player_analysis_lib.js", "text/javascript"))
    //         return true;
    // 
    //     if (hookUrl(job, url, "liveplayer.js", L"G:\\test\\web_test\\cctv\\liveplayer.js", "text/javascript"))
    //         return true;
    // 
    //     if (hookUrl(job, url, "video.a8d03c74.js", L"G:\\test\\web_test\\mse_api\\bilibili.video.a8d03c74.js", "text/javascript"))
    //         return true;
    // 

    //     if (hookUrl(job, url, "de3c81144eb2a2d6c957.js", L"G:\\test\\web_test\\mgtv\\de3c81144eb2a2d6c957.js", "text/javascript"))
    //         return true;
    //     if (hookUrl(job, url, "b6a45a2bc95a8d3e941a.js", L"G:\\test\\web_test\\mgtv\\b6a45a2bc95a8d3e941a.js", "text/javascript"))
    //         return true;
    //     if (hookUrl(job, url, "ed27ed969f72424e4a4b.js", L"G:\\test\\web_test\\mgtv\\ed27ed969f72424e4a4b.js", "text/javascript"))
    //         return true;    
    //     if (hookUrl(job, url, "a1f373c6c9488fdc9682.js", L"G:\\test\\web_test\\mgtv\\a1f373c6c9488fdc9682.js", "text/javascript"))
    //         return true;
    //     if (hookUrl(job, url, "promise.polyfill.min.js", L"G:\\test\\web_test\\mgtv\\promise.polyfill.min.js", "text/javascript"))
    //         return true;

    if (hookUrl(job, url, "hls.min.js", L"G:\\test\\web_test\\mgtv\\hls.min.js", "text/javascript"))
        return true;
    if (hookUrl(job, url, "vplayer.min.js?ver", L"G:\\test\\web_test\\mgtv\\vplayer.min.js", "text/javascript"))
        return true;
    if (hookUrl(job, url, "hls-loader.min.js", L"G:\\test\\web_test\\mgtv\\hls-loader.min.js", "text/javascript"))
        return true;

    //     if (hookUrl(job, url, "mguser.api.max.mgtv.com/user/getArtistsByMediaId", L"G:\\test\\web_test\\mgtv\\getArtistsByMediaId.js", "text/javascript"))
    //         return true;

    //     if (strstr(url, "daxiansheng/images/header.png")) {
    //         //mbNetSetData(job, " ", 1);
    //         OutputDebugStringA("trackMi.tml!!!!!!!!!!!!!!!!!!\n");
    //         return false;
    //     }

    //     std::vector<char> buffer;
    //     readFile(L"E:\\test_code\\bemo\\res\\1.asar", &buffer);
    // 
    //     static void* s_packetMgr = nullptr;
    //     if (!s_packetMgr)
    //         s_packetMgr = wkeCreateResPacketMgr(buffer.data(), buffer.size());
    // 
    //     wkeOnUrlBeginResPacketMgr(s_packetMgr, url, job);

    //     if (hookUrl(job, url, "wangeditor@3.1.1/release/wangEditor.min.js", L"G:\\test\\web_test\\wangeditor\\wangeditor.js", "text/javascript"))
    //         return true;

    //      if (0 != strstr(url, "pcweb.api.mgtv.com/abpage?did=")) {
    //          mbNetHookRequest(job);
    //      }
    //      if (0 != strstr(url, "mguser.api.max.mgtv.com/user/getArtistsByMediaId")) {
    //          mbNetHookRequest(job);
    //      }

    //     if (0 != strstr(url, "video.webm")) {
    //         mbNetChangeRequestUrl(job, "file:///G:/test/web_test/icoc/index-4.mp4");
    //     }

    return false;
}

static void MB_CALL_TYPE handleLoadUrlEnd(mbWebView webView, void* param, const char* url, void* job, void* buf, int len)
{
    saveDumpFile(L"G:\\test\\web_test\\mgtv\\abpage.js", (const char*)buf, len);
    // 
    //     std::vector<char> buffer;
    //     readFile(L"G:\\test\\web_test\\m_baidu\\index.htm", &buffer);
    //     mbNetSetData(job, &buffer[0], buffer.size());

    std::vector<char> buffer(len + 1);
    memcpy(buffer.data(), buf, len);
    buffer.push_back('\0');

    //     if (nullptr != strstr(buffer.data(), "Promises must be constructed via new"))
    //         OutputDebugStringA("find!\n");
}