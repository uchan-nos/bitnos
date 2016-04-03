/*
 * uncopyable.h
 *
 *  Created on: 2009/07/12
 *      Author: uchan
 */

#ifndef UNCOPYABLE_H_
#define UNCOPYABLE_H_

/*
 * クラスのコピーを禁止するクラス
 */
class Uncopyable
{
    private:
            /*
             * コピー（代入を含む）は禁止する
             */
            Uncopyable(const Uncopyable&);
                Uncopyable& operator=(Uncopyable&);
    protected:
              /*
               * 派生クラスのオブジェクトの生成と破棄は許可する
               */
              Uncopyable() {}
                  ~Uncopyable() {}
};

#endif /* UNCOPYABLE_H_ */
