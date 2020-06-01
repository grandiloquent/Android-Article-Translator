package euphoria.psycho.application;

public class NativeUtils {
    static {
        System.loadLibrary("native-lib");
    }

    public static native String baiduTranslate(String word, boolean englishToChinese);

    public static native String googleTranslate(String word, boolean englishToChinese);

    public static native String postJson(String host, String port, String path, String headers, String body);


    public static native String removeRedundancy(String text);

    public static native String uploadFile(String filePath);

    public static native String executeCommand(String cmd);

    public static native String insertNote(String jsonBody);

    public static native String upadteNote(String jsonBody);

    public static native String youdaoDictionary(String word, boolean translate, boolean englishToChinese);
}
