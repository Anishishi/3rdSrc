氏名：西山　晃人
学籍番号：03-190503
作品名：ストライクボール自動判定機
作品紹介：テレビ中継されるアングルで保存した野球の映像を用いて、ストライクボールを自動で
            判定する装置を作った。処理過程では、大津の二値化、Haar-like分類器、HOG特徴分類器、
            SVM、Hough変換などの手法を用いて実装した。
実行環境：Ubuntu18.04、OpenCV3.2.0
実行方法：  make final
            ./final 「inputファイル名」
                で実行可能。動画の再生が開始され、ストライクボール等が表示される。
                inputファイル名は「input00.mp4, input01.mp4, 02, 03, 10, 12, 13」がある。
操作説明：s:一時停止、q or Q:中断、Enter:表示画面の保存
            判定が出されたときに一時停止するのでsで再び再生させる必要あり。映像終了時にも
            停止されるのでいづれかのキーで終了させる必要あり。
