# kg-cpp
c++ go style library

# why
go有許多不錯的玩意 然有時不得不用 c++ 孤期望在c++中 亦能愉快的玩這些玩意 特別是和c++11一起玩時

幾日前玩了下 boost::asio 的協程後 感到 如果用上 boost 只需要 封裝go幾個 簡單的 特性 就能在c++中 如同go一樣玩 而且能得到比go更高的 效率

當然 c++畢竟不是 go 方便程度依然無法和go相比 效率上的提升 和代碼的增加 亦不一定值得 然如果在某些條件下無法使用 go 或者 就需要這提升的 效率 將go風格 移植到 c++ 是不錯的選擇

# document
api文檔 使用doxygen生成 請自行安裝 相應工具(類似如下命令)

sudo apt install doxygen graphviz

doxygen ./document.cnf
