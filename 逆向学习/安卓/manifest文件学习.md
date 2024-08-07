# AndroidManifest.xml
> 文件是一个Android应用程序中的重要文件之一，用于描述应用程序的基本属性、组件、权限等信息。它必须位于应用程序的根目录下，并且必须命名为AndroidManifest.xml。
> AndroidManifest.xml文件采用XML格式编写，它由若干个标签和属性组成，每个标签代表一个组件或权限，属性则用于描述组件或权限的属性。

## <manifest>标签
> <manifest>标签是AndroidManifest.xml文件的根标签，它包含了整个应用程序的配置信息，如包名、版本号、权限等。它的主要属性有：
1. package：应用程序的包名，必须是唯一的，通常使用域名的倒序来命名，如com.example.myapp。
2. versionCode：应用程序的版本号，整数，用于标识应用程序的版本，每次发布新版本时，该值都必须递增。
3. versionName：应用程序的版本名称，字符串，用于标识应用程序的版本，每次发布新版本时，该值可以不变。
4. platformBuildVersionCode：Android系统的版本号，整数，用于标识Android系统的版本，每次发布新版本时，该值都必须递增。
5. platformBuildVersionName：Android系统的版本名称，字符串，用于标识Android系统的版本，每次发布新版本时，该值可以不变。
6. installLocation：应用程序安装位置，字符串，用于指定应用程序安装的位置，可选值有auto、internalOnly、preferExternal，分别表示自动选择、仅安装在内部存储器、优先安装在外部存储器。
7. sharedUserId：共享用户ID，字符串，用于指定应用程序的共享用户ID，如果指定了该属性，则应用程序将与其他具有相同共享用户ID的应用程序共享数据。
8. sharedUserLabel：共享用户标签，字符串，用于指定应用程序的共享用户标签，如果指定了该属性，则应用程序将与其他具有相同共享用户标签的应用程序共享数据。
9. debuggable：是否允许调试，布尔值，用于指定应用程序是否允许调试，如果设置为true，则可以使用DDMS工具进行调试，如果设置为false，则不能使用DDMS工具进行调试。
10. hasCode：是否包含代码，布尔值，用于指定应用程序是否包含代码，如果设置为true，则表示应用程序包含代码，如果设置为false，则表示应用程序不包含代码。
11. testOnly：是否仅用于测试，布尔值，用于指定应用程序是否仅用于测试，如果设置为true，则表示应用程序仅用于测试，如果设置为false，则表示应用程序不仅用于测试。
12. allowBackup：是否允许备份，布尔值，用于指定应用程序是否允许备份，如果设置为true，则表示应用程序允许备份，如果设置为false，则表示应用程序不允许备份。
13. backupAgent：备份代理，字符串，用于指定应用程序的备份代理，如果设置了该属性，则表示应用程序的数据将由该代理进行备份。
14. supportsRtl：是否支持RTL，布尔值，用于指定应用程序是否支持RTL，如果设置为true，则表示应用程序支持RTL，如果设置为false，则表示应用程序不支持RTL。
15. label：应用程序的标签，字符串，用于指定应用程序的标签，该标签将显示在应用程序的图标下方。
16. icon：应用程序的图标，字符串，用于指定应用程序的图标，该图标将显示在应用程序的标签下方。
17. banner：应用程序的横幅，字符串，用于指定应用程序的横幅，该横幅将显示在应用程序的标签和图标下方。
18. roundIcon：应用程序的圆形图标，字符串，用于指定应用程序的圆形图标，该图标将显示在应用程序的标签和图标下方。
19. description：应用程序的描述，字符串，用于指定应用程序的描述，该描述将显示在应用程序的标签、图标和横幅下方。
20. theme：应用程序的主题，字符串，用于指定应用程序的主题，该主题将作为应用程序的默认主题。
21. vmSafeMode：是否启用VM安全模式，布尔值，用于指定应用程序是否启用VM安全模式，如果设置为true，则表示应用程序启用VM安全模式，如果设置为false，则表示应用程序不启用VM安全模式。

