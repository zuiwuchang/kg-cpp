#ifndef KG_CRYPTO_K3XSX_SALT_T_HEADER_HPP
#define KG_CRYPTO_K3XSX_SALT_T_HEADER_HPP

#include "../types.hpp"
namespace kg
{
	namespace crypto
	{
/**
*   \brief k3xsx_salt_t 加密算法
*
*   k3xsx_salt_t 是孤設計的一個古典加密算法 用於 快速 將數據 編碼爲 不太容易 被解開的 密文\n
*   salt 顯示了 隨機 撒鹽 以使相同數據 隨機 編碼爲不同的 數據 從而 避免一些 差量分析的逆向 行爲\n
*   3xsx 顯示了 編碼 將 通過 三步 運算\n
*   1	產生 3個 隨機的 鹽 salts=[0,1,2] 並將 鹽值+一個固定值 salt\n
*   2	對 待加密的每個 字節 進行 xsx 三步運算\n
*   3	將 步驟1產生的鹽 salts 寫入輸出數據頭部 將加密後的數據 寫到 鹽後\n\n
*   xsx\n
*   	//step 0\n
*   	b = b xor (salts[0] + salt) //異或\n
*   	//step 1\n
*   	swapbit(b,0,7)	//交換 字節 第0bit 與 第7bit 的 值\n
*   	swapbit(b,1,6)\n
*   	swapbit(b,2,5)\n
*   	swapbit(b,3,4)\n
*   	b = b shl-loop (salts[1] + salt) // shr-loop 左移 但 越位的數據 不捨棄 而是作爲最低位的 補位數據\n
*   	//step 2\n
*   	b = b xor (salts[2] + salt)\n
*/
class k3xsx_salt_t
{
private:
    /**
    *   \brief  鹽基值
    */
    kg::byte_t _salt;

    /**
    *   \brief  返回 鹽 長度
    */
    static std::size_t salt_len()
    {
        return 3;
    }
public:
    /**
    *   \brief  構造
    *   \param salt 鹽基值
    */
    k3xsx_salt_t(const kg::byte_t salt=0)
        :_salt(salt)
    {
    }



    /**
    *   \brief  加密數據
    *
    *   加密後的數據 會比原數據 長3字節(存儲隨機產生的 鹽)
    *
    *   \param src     待加密的原數據
    *   \param n       src長度
    *   \param dist    加密後的輸出數據 長度必須 >= n+3
    */
    void encryption(const kg::byte_t* src,std::size_t n,kg::byte_t* dist)
    {
        //創建隨機鹽
        _rand_salts(dist);

        //對每個字節 進行 加密
        std::size_t saltLen = salt_len();
        for(std::size_t i = 0; i < n; ++i)
        {
           dist[saltLen+i] = _encryption(src[i],dist);
        }
    }
    /**
    *   \brief  解密數據
    *
    *   解密後的數據 會比原數據 短3字節(去掉 鹽)
    *
    *   \param src     待解密的原數據
    *   \param n       src長度    n必須 >=3
    *   \param dist    解密後的輸出數據
    */
    bool decryption(const kg::byte_t* src,std::size_t n,kg::byte_t* dist)
    {
        const std::size_t saltLen = salt_len();
        if(n < saltLen)
        {
            return false;
        }
        else if(n == saltLen)
        {
            return true;
        }

        //對每個字節 進行 解密
        for(std::size_t i = 0; i < n - saltLen;++i)
        {
                dist[i] = _decryption(src[i+saltLen], src);
        }

        return true;
    }
private:
    /**
    *   \brief  創建隨機的鹽
    *
    *   \param salts    存儲產生的鹽
    */
    void _rand_salts(kg::byte_t* salts)
    {
        srand(time(NULL));

        std::size_t n = salt_len();
        for(std::size_t i=0;i<n;++i)
        {
            unsigned int x = (unsigned int)rand();
            salts[i] = (kg::byte_t)(x%255);
        }
    }
    /**
    *   \brief  逐字節加密
    *
    *   \param b        等加密 字節
    *   \param salts    3個隨機 鹽
    *   \return         加密後的字節
    */
    kg::byte_t _encryption(const kg::byte_t b,const kg::byte_t* salts)
    {

        //step 0
        kg::byte_t c = b ^ (salts[0] + _salt);

        //step 1
        c = _swap_bit(c,0,7);
        c = _swap_bit(c,1,6);
        c = _swap_bit(c,2,5);
        c = _swap_bit(c,3,4);
        c = _shl_loop(c,salts[1] + _salt);

        //step 2
        c ^= salts[2] + _salt;

        return c;
    }

    /**
    *   \brief  逐字節解密
    *
    *   \param b        等解密 字節
    *   \param salts    3個隨機 鹽
    *   \return         解密後的字節
    */
    kg::byte_t _decryption(const kg::byte_t b,const kg::byte_t* salts)
    {
       //step 2
        kg::byte_t c = b ^ (salts[2] + _salt);

        //step 1
        c = _shr_loop(c,salts[1] + _salt);
        c = _swap_bit(c,0,7);
        c = _swap_bit(c,1,6);
        c = _swap_bit(c,2,5);
        c = _swap_bit(c,3,4);

        //step 0
        c ^= (salts[0] + _salt);
        return c;
    }

    kg::byte_t _swap_bit(const kg::byte_t b,std::size_t l,std::size_t r)
    {
        if(l==r)
        {
            return b;
        }

        kg::byte_t c = b;
        if(_is_bit(b,l))
        {
            c = _set_bit(c,r,true);
        }
        else
        {
            c = _set_bit(c,r,false);
        }

        if(_is_bit(b,r))
        {
            c = _set_bit(c,l,true);
        }
        else
        {
            c = _set_bit(c,l,false);
        }

        return c;
    }
    bool _is_bit(const kg::byte_t b,std::size_t n)
    {
        kg::byte_t c = 1;
        c<<=n;
        return c & b;
    }
    kg::byte_t _set_bit(const kg::byte_t b,std::size_t n,bool ok)
    {
        if(ok)
        {
            kg::byte_t c = 1;
            c<<=n;
            return b | c;
        }

        kg::byte_t c = 1;
        c<<=n;
        return b & (~c);
    }
    kg::byte_t _shl_loop(const kg::byte_t b,std::size_t n)
    {
        n %=8;
        if(n == 0)
        {
            return b;
        }

        kg::byte_t c = b << n;
        c |= b >> (8-n);

        return c;
    }
    kg::byte_t _shr_loop(const kg::byte_t b,std::size_t n)
    {
        n %=8;
        if(n == 0)
        {
            return b;
        }

        kg::byte_t c = b >> n;
        c |= b << (8-n);

        return c;
    }
};

	};
};
#endif	//KG_CRYPTO_K3XSX_SALT_T_HEADER_HPP
