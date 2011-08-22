#ifndef PLACE_MACRO_H
#define PLACE_MACRO_H

#define Q_DECLARE_D_FUNC(Class) \
    inline Class##Private* d_func(); \
    inline const Class##Private* d_func() const;\
    friend class Class##Private;

#define Q_DECLARE_COPY_CTOR(Class, BaseClass) \
    Class(const BaseClass &other);

#define Q_IMPLEMENT_D_FUNC(Class) \
    Class##Private* Class::d_func() { return reinterpret_cast<Class##Private *>(d_ptr.data()); } \
    const Class##Private* Class::d_func() const { return reinterpret_cast<const Class##Private *>(d_ptr.constData()); }

#define Q_IMPLEMENT_COPY_CTOR(Class, BaseClass) \
    Class::Class(const BaseClass& other) : BaseClass() { Class##Private::copyIfPossible(d_ptr, other); }

#define Q_DEFINE_PRIVATE_HELPER(Class, BaseClass, ClassType, basename) \
    BaseClass##Private* clone() const { return new Class##Private(*this); } \
            virtual BaseClass::Type type() const {return ClassType;} \
    static void copyIfPossible(QSharedDataPointer<BaseClass##Private>& d_ptr, const BaseClass &other) \
    { \
        if (other.basename##Type() == ClassType) \
            d_ptr = extract_d(other); \
        else \
            d_ptr = new Class##Private; \
    }

#endif
