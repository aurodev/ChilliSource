//
//  Keyboard.cpp
//  Chilli Source
//  Created by Ian Copland on 25/03/2014.
//
//  The MIT License (MIT)
//
//  Copyright (c) 2014 Tag Games Limited
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#include <ChilliSource/Input/Keyboard/Keyboard.h>

#ifdef CS_TARGETPLATFORM_ANDROID
#   include <ChilliSource/Backend/Platform/Android/Input/Keyboard/Keyboard.h>
#endif

#ifdef CS_TARGETPLATFORM_IOS
#   include <ChilliSource/Backend/Platform/iOS/Input/Keyboard/Keyboard.h>
#endif

namespace ChilliSource
{
    namespace Input
    {
        CS_DEFINE_NAMEDTYPE(Keyboard);
        //-------------------------------------------------------
        //-------------------------------------------------------
        KeyboardUPtr Keyboard::Create()
        {
#if defined CS_TARGETPLATFORM_ANDROID
            return KeyboardUPtr(new Android::Keyboard());
#elif defined CS_TARGETPLATFORM_IOS
            return KeyboardUPtr(new iOS::Keyboard());
#else
            return nullptr;
#endif

        }
    }
}
