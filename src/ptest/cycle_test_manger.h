/*
 * cycle_test_manger.h
 *
 *  Created on: 2014Äê12ÔÂ30ÈÕ
 *      Author: ranger.shi
 */

#ifndef COIN_PTEST_CYCLE_TEST_MANGER_H_
#define COIN_PTEST_CYCLE_TEST_MANGER_H_

#include "cycle_test_base.h"

class CycleTestManger {
 public:
	CycleTestManger() {
	};
	void Initialize();
	void Initialize(vector<std::shared_ptr<CycleTestBase> > &refvTestIn);
	void Run();
	static CycleTestManger &GetNewInstance() {
		static CycleTestManger sInstance;
		return sInstance;
	}

	virtual ~CycleTestManger() {
	};

 private:
	vector<std::shared_ptr<CycleTestBase> > m_vcTest;
};

#endif /* COIN_PTEST_CYCLE_TEST_MANGER_H_ */
