package euphoria.psycho.application;

import android.content.ClipData;
import android.text.Editable;
import android.widget.EditText;

import java.security.SecureRandom;
import java.util.Arrays;
import java.util.regex.Pattern;

public class Helper {
    // 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ
    static final String AB = "abcdefghijklmnopqrstuvwxyz";
    static SecureRandom rnd = new SecureRandom();

    public static void formatCode(EditText editText) {
//        String content = editText.getText().subSequence(
//                editText.getSelectionStart(),
//                editText.getSelectionEnd()).toString();
//
//        if (Share.isNullOrWhiteSpace(content)) {
//            content = Share.getClipboardText(editText.getContext());
//        }
//        editText.getText().replace(
//                editText.getSelectionStart(),
//                editText.getSelectionEnd(),
//                "`" + content + "`"
//        );
        selectLine(editText);
        Share.replaceSelectionText(editText,
                Share.getSelectionText(editText).toString()
                        .replaceAll("[a-zA-Z0-9:.*=/ <>$_\\-,(){}!\"&+\\[\\]]{2,}",
                                "`$0`"));
    }

    public static int getLineStart(EditText editText) {

        String text = editText.getText().toString();
        int len = text.length();
        if (len == 0) return 0;

        int start = editText.getSelectionStart();
        int end = editText.getSelectionEnd();
        if (start == len) {
            start--;
        }
        if (start == end && text.charAt(start) == '\n') {
            if (start != 0)
                start--;

            while (start > 0 && text.charAt(start) != '\n') {
                start--;
            }
            if (text.charAt(start) == '\n') {
                start++;
            }

        } else {


            while (start > 0 && text.charAt(start) != '\n') {
                start--;
            }
            if (text.charAt(start) == '\n') {
                start++;
            }


        }
        return start;
    }

    public static void insertAfter(EditText editText, String text) {
        int end = editText.getSelectionEnd();
        editText.getText().insert(end, text);
        editText.setSelection(end);
    }

    public static void insertBefore(EditText editText, String text) {
        int start = editText.getSelectionStart();
        editText.getText().insert(start, text);
        editText.setSelection(start);
    }

    public static boolean isWhitespace(EditText editText) {
        Editable editable = editText.getText();
        if (editable.length() == 0) return true;
        for (int i = 0, j = editable.length(); i < j; i++) {
            if (!Character.isWhitespace(editable.charAt(i))) {
                return false;
            }
        }
        return true;
    }

    public static String selectLine(EditText editText) {
        if (isWhitespace(editText)) return null;
        Editable text = editText.getText();
        int len = text.length();
        int start = editText.getSelectionStart();
        int end = editText.getSelectionEnd();
        if (start == len) {
            start--;
        }
        while (start > 0 && text.charAt(start - 1) != '\n') {
            start--;
        }
        while (end + 1 < len && text.charAt(end) != '\n') {
            end++;
        }
//        char c=text.charAt(end);
//        String v=text.subSequence(start, end).toString();;
        if (end < len && text.charAt(end) != '\n') {
            end++;
        }
        editText.setSelection(start, end);
        return text.subSequence(start, end).toString();
//
//        int len = text.length();
//
//        int start = editText.getSelectionStart();
//        int end = editText.getSelectionEnd();
//        if (start == len) {
//            start--;
//        }
//        if (start == end && text.charAt(start) == '\n') {
//            start--;
//
//            while (start > 0 && text.charAt(start) != '\n') {
//                start--;
//            }
//            if (text.charAt(start) == '\n') {
//                start++;
//            }
//
//        } else {
//
//
//            while (start > 0 && text.charAt(start) != '\n') {
//                start--;
//            }
//            if (text.charAt(start) == '\n') {
//                start++;
//            }
//
//            while (end < len && text.charAt(end) != '\n') {
//                end++;
//            }
//
//
//        }
//        editText.setSelection(start, end);
//
//        // String str=text.substring(start, end);
//
//        return text.substring(start, end);
    }

    public static String toggleBold(String text) {
        if (text.startsWith("**") && text.endsWith("**")) {
            text = Share.substringAfter(text, "**");
            return Share.substringBeforeLast(text, "**");
        }
        return "**"
                .concat(text)
                .concat("**");
    }

    public static void translate(EditText editText, int t) {

        final String q = selectLine(editText);
        if (Share.isNullOrWhiteSpace(q)) return;
        ThreadUtils.postOnBackgroundThread(() -> {

            String result = null;
            switch (t) {
                case 0:
                    result = NativeUtils.baiduTranslate(q, true);
                    break;
                case 1:
                    result = NativeUtils.youdaoDictionary(q, true, true);
                    if (result != null)
                        result = result
                                .replaceAll("图\\d+-\\d+", "下图中")
                                .replace("(", "（")
                                .replace(")", "）")
                                .replace(";", "；")
                                .replace(":", "：")
                                .replace("-", "——")
                                .replace("?", "？")
                                ;
                    break;
                case 2:
                    result = NativeUtils.googleTranslate(q, true);
                    break;
            }
            if (result == null) {
                Share.message(editText.getContext(), "Translate is null");
                return;
            }


            String finalResult = result.replace("您", "你");
            ThreadUtils.postOnUiThread(() -> insertAfter(editText, "\n\n" +
                    finalResult
            ));

        });
    }

    static void breakLine(EditText editText) {

        final String q = selectLine(editText);
        if (Share.isNullOrWhiteSpace(q)) return;
        String[] pieces = q.split("(?<!\\.)[.?] *(?=[a-zA-Z0-9])");
        editText.getText().replace(
                editText.getSelectionStart(),
                editText.getSelectionEnd(),
                Share.join(".\n\n", Arrays.asList(pieces))
        );
    }

    static void cutBlock(EditText editText) {
        int current = editText.getSelectionStart();

        Editable content = editText.getText();
        if (content.charAt(current) == '\n') current++;

        if (content.charAt(current) == '#'
                && current + 2 < content.length() - 1 && content.charAt(current + 1) == '#'
                && content.charAt(current + 2) == ' ') {
            int start = 0;
            int end = start;
            while (end < content.length()) {
                if (content.charAt(end) == '\n'
                        && end + 3 < content.length() - 1
                        && content.charAt(end + 1) == '#'
                        && content.charAt(end + 2) == '#'
                        && content.charAt(end + 3) == ' ') {
                    break;
                }
                end++;
            }

            Share.setClipboardText(editText.getContext(), content.subSequence(start, end).toString());

            content.replace(start, end, "");

        }
    }

    static CharSequence deleteExtend(EditText editText) {
        Editable text = editText.getText();
        int len = text.length();
        if (len == 0) return null;
        int start = editText.getSelectionStart();
        int end = editText.getSelectionEnd();
        if (start == len) {
            start--;
        }
        boolean found = false;
        for (int i = start; i >= 0; i--) {
            char c = text.charAt(i);
            if (c == '\n') {
                for (int j = i - 1; j >= 0; j--) {
                    c = text.charAt(j);
                    if (!Character.isWhitespace(c)) break;
                    if (c == '\n') {
                        start = j;
                        found = true;
                        break;
                    }
                }
            }
            if (found) break;
        }
        if (!found) {
            start = 0;
        }
        found = false;
        for (int i = end; i < len; i++) {
            char c = text.charAt(i);
            if (c == '\n') {
                for (int j = i + 1; j < len; j++) {
                    c = text.charAt(j);
                    if (!Character.isWhitespace(c)) break;
                    if (c == '\n') {
                        end = j + 1;
                        found = true;
                        break;
                    }
                }
            }
            if (found) break;
        }
        if (!found) {
            end = len;
        }
        CharSequence value = text.subSequence(start, end);
        text.replace(start, end, "\n");
        return value;
    }

    static CharSequence deleteLineStrict(EditText editText) {
        Editable text = editText.getText();
        int len = text.length();
        if (len == 0) return null;
        int start = editText.getSelectionStart();
        int end = editText.getSelectionEnd();
        if (start == len || text.charAt(start) == '\n') {
            start--;
        }
        boolean found = false;
        for (int i = start; i >= 0; i--) {
            char c = text.charAt(i);
            if (c == '\n') {
                for (int j = i - 1; j >= 0; j--) {
                    c = text.charAt(j);
                    if (!Character.isWhitespace(c)) {
                        start = j + 1;
                        found = true;
                        break;
                    }
                }
            }
            if (found) break;
        }
        if (!found) {
            start = 0;
        }
        found = false;
        for (int i = end; i < len; i++) {
            char c = text.charAt(i);
            if (c == '\n') {
                for (int j = i + 1; j < len; j++) {
                    c = text.charAt(j);
                    if (!Character.isWhitespace(c)) {
                        end = j;
                        found = true;
                        break;
                    }
                }
            }
            if (found) break;
        }
        if (!found) {
            end = len;
        }
        CharSequence value = text.subSequence(start, end);
        text.delete(start, end);
        return value;
    }


    /*
     *
     * 从光标点向前匹配第一个换行符，匹配成功后，继续向前匹配非空字符，匹配成功后停止
     * 以同样的道理向后匹配
     * */
    static CharSequence deleteLineWithWhitespace(EditText editText) {
        Editable text = editText.getText();
        int len = text.length();
        if (len == 0) return null;
        int start = editText.getSelectionStart();
        int end = editText.getSelectionEnd();
        if (start == len || text.charAt(start) == '\n') {
            start--;
        }
        if (end < len && text.charAt(end) == '\n' && end - 1 > -1) {
            end--;
        }
        while (start - 1 > -1 && text.charAt(start - 1) != '\n') start--;
        while (start - 1 > -1 && Character.isWhitespace(text.charAt(start - 1))) start--;
        while (end + 1 < len && text.charAt(end + 1) != '\n') end++;
        while (end + 1 < len && Character.isWhitespace(text.charAt(end + 1))) end++;
        if (end + 1 < len) end++;
        CharSequence charSequence = editText.getText().subSequence(start, end);
        editText.getText().replace(start, end, "\n\n");
        return charSequence;
    }

    static int[] extendSelect(EditText editText) {
        String value = editText.getText().toString();
        if (value.length() == 0) {
            return new int[2];
        }
        int len = value.length();

        int start = editText.getSelectionStart();
        int end = editText.getSelectionEnd();
        if (start == end) {
            start--;
        }
        while (start > 0 && !(value.charAt(start) == '\n' && value.charAt(start - 1) == '\n')) {
            start--;
        }
        len = len - 1;
        while (end < len && !(value.charAt(end) == '\n' && value.charAt(end + 1) == '\n')) {
            end++;

        }

        if (value.charAt(start) == '\n') start++;

        if (end + 1 == len) end = len;
        return new int[]{
                start, end
        };

    }

    static void formatList(EditText editText) {


        int[] position = extendSelect(editText);
        editText.setSelection(position[0], position[1]);
        String value = editText.getText().toString().substring(position[0], position[1]).trim();
        String[] lines = value.split("\n");
        StringBuilder sb = new StringBuilder();
        for (String l : lines) {
            if (Share.isNullOrWhiteSpace(l)) continue;
            if (l.startsWith("* ")) {
                sb.append(l.substring(2)).append('\n');
            } else {

                sb.append("* ").append(l).append('\n');
            }
        }
        editText.getText().replace(editText.getSelectionStart(), editText.getSelectionEnd(), sb.toString());

    }

    static void formatTitle(EditText editText) {


        int start = getLineStart(editText);


        Editable value = editText.getText();
        editText.setSelection(start);

        if (value.length() == 0 || value.charAt(start) != '#') {
            insertBefore(editText, "## ");
        } else {
            insertBefore(editText, "#");

        }
    }

    static String randomString(int len) {
        StringBuilder sb = new StringBuilder(len);
        for (int i = 0; i < len; i++)
            sb.append(AB.charAt(rnd.nextInt(AB.length())));
        return sb.toString();
    }

    static String removeRedundancyLines(String text) {
        String[] pieces = text.trim().split("\n");
        StringBuilder sb = new StringBuilder();
        for (int i = 0, j = pieces.length; i < j; i++) {
            String piece = pieces[i];
            if (Share.isNullOrWhiteSpace(piece)) {
                sb.append('\n');
                while (i + 1 < j && Share.isNullOrWhiteSpace(pieces[i + 1])) {
                    i++;
                }
            } else {
                sb.append(piece).append('\n');
            }
        }
        return sb.toString();
    }


}
