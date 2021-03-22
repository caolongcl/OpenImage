package com.cwdx.opensrc.av.core;

import android.content.Context;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

class AssetCopier {
    public static void CopyAllAssets(Context context, String destination) {
        copyAssetsToDst(context, "", destination);
    }

    private static void copyAssetsToDst(Context context, String srcPath, String dstPath) {
        try {
            String fileNames[] = context.getAssets().list(srcPath);
            if (fileNames.length > 0) {
                File file = new File(dstPath);
                file.mkdirs();
                for (String fileName : fileNames) {
                    if (!"".equals(srcPath)) {
                        copyAssetsToDst(context, srcPath + "/" + fileName, dstPath + "/" + fileName);
                    } else {
                        copyAssetsToDst(context, fileName, dstPath + "/" + fileName);
                    }
                }
            } else {
                InputStream is = context.getAssets().open(srcPath);
                FileOutputStream fos = new FileOutputStream(new File(dstPath));
                byte[] buffer = new byte[1024];
                int byteCount = 0;
                while ((byteCount = is.read(buffer)) != -1) {
                    fos.write(buffer, 0, byteCount);
                }
                fos.flush();
                is.close();
                fos.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
