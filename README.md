# xcode-build-test-ddd

このリポジトリは、C/C++のライブラリをxcodeプロジェクト同梱で配布する際に、
ライブラリ間の依存関係をどのように管理するのが良いか調べたものである。

4つのモジュールにリポジトリを分けている。

- aaa: ライブラリ。依存先は無い。
- bbb: ライブラリ。aaaに依存している。
- ccc: ライブラリ。aaaに依存している。
- ddd: アプリ。bbbとcccに依存している。

このような状況で、dddをビルドする際に、bbbが依存するaaaと、cccが依存するaaaを同一化したい。

xcodeのstatic libraryプロジェクトには、ビルドディレクトリにヘッダーを書き出す機能がある。
プロジェクトをセットアップするとデフォルトで設定されていて、
Build PhaseのProjectヘッダーからPublicヘッダーに移し、
Public Headers Folder Pathを `include/aaa` などにすると、
ビルドディレクトリにヘッダーが展開される。
ここで、ヘッダーのディレクトリ構造は維持されず、`.h`　ファイルがフラットに並ぶ形に制限される。

デフォルトではビルドディレクトリ直下の `include` はコンパイラにサーチパスとして指定されているので、
上記のようにするだけで、 `#include <aaa/aaa.h>` が通るようになる。

そして、bbbとcccは、Link Binaryとしてaaaを指定することによって、
プロジェクトの依存関係が設定されて、bbbがaaaに依存することをxcodeが検出できる。

Link Binaryの指定は、内部的には名前しか見ていないようで、
xcworkspaceに共存するaaaであれば、bbbのプロジェクトとの相対パスが変わっていても、
連携してくれる。

これで、bbbの開発時はbbb内部にサブモジュールで取り入れたaaaを利用して、
dddの開発時にはddd内部にbbbとaaaをそれぞれサブモジュールで入れて、
bbbはそちらのaaaを使ってビルドする、ということができる。

これで概ね目的が達成できているが、微妙な点がある。
それは、このやり方だと bbb のスタティックライブラリに aaa のバイナリも結合されてしまうことだ。
bbbもcccもそれぞれ冗長にaaaの実行コードを含んでしまう。
リンカーが削除してくれるし、同名のオブジェクトファイルを混ぜることもできるので、
動くには動くが、あまりきれいではない。

この設定は本当にスタティックライブラリ通しを結合して１つのスタティックライブラリにしたい場合にやるもので、
今やりたいのは aaa, bbb, ccc のライブラリを作ってアプリのところで一つにすることだ。
だから本当はstatic library同士の依存関係は保持しつつ、リンクしない状態にしたい。
しかしもし、Link Binaryの指定をはずすと、ビルド依存順序が不定になる。
そうすると、aaaのビルドがbbbより後回しになることがあり、
その場合aaaのヘッダがビルドディレクトリに展開されないのでincludeできなくてコンパイルエラーになったりする。

スキームの設定で、Find Implicit Dependenciesを外して、手動で追加して、
Parallize Buildも外せば、順番にターゲットをビルドするので解決するが、
この設定自体はスキームごとなので、ライブラリ側で事前に保持しておくことができないからスケールしないし、
Parallizeできないのは苦しい。






