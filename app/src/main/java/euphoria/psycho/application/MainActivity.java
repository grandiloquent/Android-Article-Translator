package euphoria.psycho.application;

import android.Manifest;
import android.Manifest.permission;
import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.view.ContextThemeWrapper;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;


public class MainActivity extends Activity {
    private static final String KEY_STRINGS = "strings";
    private static final int REQUEST_PERMISSIONS_CODE = 1 << 1;
    private View mFormatList;
    private View mFormatTitle;
    private EditText mEditText;
    private View mTranslateBaidu;
    private View mTranslateGoogle;
    private View mTranslateYoudao;
    private View mFormat;
    private View mFormatCut;
    private SharedPreferences mPreferences;
    private View mFormatCode;
    private LinearLayout mStrings;
    private View mSubtitle;
    private View mFormatBreakLine;
    private View mFormatRemoveLine;
    private View mFormatImage;
    private View mFormatBold;
    private View mFormatSearch;
    private View mFormatCodeSplit;
    private View mFormatLink;
    private View mFormatRemoveBefore;

    private void initialize() {

        setContentView(R.layout.activity_main);
        mPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mStrings = findViewById(R.id.strings);
        mFormatTitle = findViewById(R.id.format_title);
        mFormatList = findViewById(R.id.format_list);
        mEditText = findViewById(R.id.edit_text);

        mTranslateBaidu = findViewById(R.id.translate_baidu);
        mTranslateYoudao = findViewById(R.id.translate_youdao);
        mTranslateGoogle = findViewById(R.id.translate_google);
        mFormatCut = findViewById(R.id.format_cut);
        mFormat = findViewById(R.id.format);
        mFormatCode = findViewById(R.id.format_code);
        mSubtitle = findViewById(R.id.subtitle);
        mFormatRemoveLine = findViewById(R.id.format_remove_line);
        mFormatImage = findViewById(R.id.format_image);
        mFormatBold = findViewById(R.id.format_bold);
        mFormatSearch = findViewById(R.id.format_search);
        mFormatLink = findViewById(R.id.format_link);
        mFormatCodeSplit = findViewById(R.id.format_code_split);
        mFormatCodeSplit.setOnClickListener(this::onFormatCodeSplitClicked);

        mFormatSearch.setOnClickListener(this::onFormatSearchClicked);
        mFormatBold.setOnClickListener(this::onFormatBoldClicked);
        mFormatImage.setOnClickListener(this::onFormatImageClicked);
        mFormatRemoveLine.setOnClickListener(this::onFormatRemoveLineClicked);
        mSubtitle.setOnClickListener(this::onSubtitleClicked);
        mFormatCode.setOnClickListener(this::onFormatCodeClicked);
        mFormatList.setOnClickListener(this::onFormatListClicked);
        mFormatTitle.setOnClickListener(this::onFormatTitleClicked);

        mFormat.setOnClickListener(this::onFormatClicked);
        mFormatLink.setOnClickListener(this::onFormatLinkClicked);


        mTranslateBaidu.setOnClickListener(this::onTranslateBaiduClicked);
        mTranslateGoogle.setOnClickListener(this::onTranslateGoogleClicked);
        mTranslateYoudao.setOnClickListener(this::onTranslateYoudaoClicked);
        mFormatCut.setOnClickListener(this::onFormatCutClicked);
        mFormatRemoveBefore = findViewById(R.id.format_remove_before);
        mFormatRemoveBefore.setOnClickListener(this::onFormatRemoveBeforeClicked);
        mEditText.setText(mPreferences.getString("content", null));
        mFormatBreakLine = findViewById(R.id.format_break_line);
        mFormatBreakLine.setOnClickListener(this::onFormatBreakLineClicked);
        try {
            updateStrings();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    private void onFormatRemoveBeforeClicked(View view) {
        String str = mEditText.getText().subSequence(0, mEditText.getSelectionStart()).toString();
        Share.setClipboardText(this, str);
        mEditText.setText(
                mEditText.getText().subSequence(mEditText.getSelectionStart(), mEditText.getText().length())

        );
    }


    private void onFormatBoldClicked(View v) {
        mEditText.getText().replace(
                mEditText.getSelectionStart(),
                mEditText.getSelectionEnd(),
                Helper.toggleBold(mEditText.getText().subSequence(
                        mEditText.getSelectionStart(),
                        mEditText.getSelectionEnd()
                ).toString().trim())
        );
    }

    private void onFormatBreakLineClicked(View v) {
        Helper.breakLine(mEditText);
    }

    private void onFormatClicked(View v) {
        mEditText.setText(Helper.removeRedundancyLines(mEditText.getText().toString()));
    }

    private void onFormatCodeClicked(View v) {
        Helper.formatCode(mEditText);
    }

    private void onFormatCodeSplitClicked(View v) {
        CharSequence c = Share.getSelectionText(mEditText);
        if (!Share.isNullOrWhiteSpace(c)) {
            String[] pieces = c.toString().split(",");
            List<String> stringList = new ArrayList<>();
            for (String p : pieces) {
                stringList.add(String.format("`%s`", p.trim()));
            }
            StringBuilder sb = new StringBuilder();
            sb.append(stringList.get(0));
            int length = stringList.size();
            if (length > 1) {
                for (int i = 1; i < length; i++) {
                    if (i + 1 == length) {
                        sb.append("和").append(stringList.get(i));
                    } else
                        sb.append("、").append(stringList.get(i));
                }
            }
            Share.setClipboardText(this, sb.toString());
        }
    }

    private void onFormatCutClicked(View v) {
        CharSequence result = Helper.deleteLineWithWhitespace(mEditText);
        if (result != null) {
            Share.setClipboardText(this, result.toString());
        }
    }

    private void onFormatImageClicked(View v) {
        Helper.selectLine(mEditText);
        String content = Share.getClipboardText(this);
        mEditText.getText().replace(
                mEditText.getSelectionStart(),
                mEditText.getSelectionEnd(),
                String.format("\n![%s](/static/articles/%s)\n\n",
                        mEditText.getText().subSequence(
                                mEditText.getSelectionStart(),
                                mEditText.getSelectionEnd()
                        ).toString().trim()
                        , content)
        );
    }

    private void onFormatLinkClicked(View v) {
        Integer.parseInt("xxx");
        String s = Share.getSelectionText(mEditText).toString().trim();
        Share.replaceSelectionText(mEditText,
                String.format("[%s](%s)", s, s));
    }

    private void onFormatListClicked(View v) {
        Helper.formatList(mEditText);
    }


    private void onFormatRemoveLineClicked(View v) {
        CharSequence result = Helper.deleteLineStrict(mEditText);
        if (result != null) {
            Share.setClipboardText(this, result.toString());
        }
    }

    private void onFormatSearchClicked(View v) {

        mEditText.setText(
                mEditText.getText().toString()
                        .replaceAll("(?<=\\n)([^\\n]*?):(?= [A-Z])", "* `$1`：\n")

        );
    }

    private void onFormatTitleClicked(View v) {
        Helper.formatTitle(mEditText);
    }

    private void onSubtitleClicked(View v) {
        Helper.cutBlock(mEditText);
    }

    private void onTranslateBaiduClicked(View v) {
        Helper.translate(mEditText, 0);

    }

    private void onTranslateGoogleClicked(View v) {
        Helper.translate(mEditText, 2);
    }

    private void onTranslateYoudaoClicked(View v) {
        Helper.translate(mEditText, 1);

    }

    private void updateStrings() throws IOException {

        File stringsFile = new File(Environment.getExternalStorageDirectory(), "strings.txt");

        if (!stringsFile.isFile()) {
            Share.writeAllText(stringsFile, "```javascript\n```go\n```");
        }
        int width = (int) Share.dp2px(this, 56);
        String[] pieces = Share.readAllText(stringsFile).split("\n+");
        for (String s : pieces) {
            Button textView = (Button) LayoutInflater.from(this).inflate(R.layout.btn, null);
            String v = s.length() > 10 ? s.substring(0, 10) : s;
            textView.setText(v);
            textView.setTag(s);
            textView.setOnClickListener(vv -> {
                Helper.insertAfter(mEditText, (String) vv.getTag());
            });
            mStrings.addView(textView);
        }
//        Set<String> strings = mPreferences.getStringSet(KEY_STRINGS, new HashSet<>());
//        if (strings.size() > 0) {
//            for (int i = 0; i < mStrings.getChildCount(); i++) {
//                if (i != 0) {
//                    mStrings.removeViewAt(i);
//                }
//            }
//            for (String s : strings) {
//                TextView textView = new TextView(this);
//                textView.setText(s);
//                textView.setOnClickListener(v -> {
//                    Helper.insertAfter(mEditText, s);
//                });
//                textView.setOnLongClickListener(v -> {
//                    strings.remove(s);
//                    mPreferences.edit().putStringSet(KEY_STRINGS, strings).apply();
//                    updateStrings();
//                    return true;
//                });
//                mStrings.addView(textView);
//            }
//        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestPermissions(new String[]{
                permission.WRITE_EXTERNAL_STORAGE
        }, REQUEST_PERMISSIONS_CODE);
    }

    @Override
    protected void onPause() {
        if (mEditText != null && !Helper.isWhitespace(mEditText))
            mPreferences.edit().putString("content", mEditText.getText().toString()).apply();

        super.onPause();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        initialize();
    }
}
