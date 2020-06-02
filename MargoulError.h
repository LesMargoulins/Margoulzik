#ifndef MARGOULERROR_H_
	#define MARGOULERROR_H_
	//#include <Arduino.h>

	class MargoulError {
		private:
			static bool _state;
		public:
			MargoulError();
			MargoulError(bool initState);
			void throwError();
			bool getState();
			void setState(bool state);

	};

#endif /* MARGOULERROR_H_ */