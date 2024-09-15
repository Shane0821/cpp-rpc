// T must be: no-throw default constructible and no-throw destructible
template <typename T>
struct Singleton {
private:
    struct object_creator {
        // This constructor does nothing more than ensure that instance()
        //  is called before main() begins, thus creating the static
        //  T object before multithreading race issues can come up.
        object_creator() { Singleton<T>::GetInst(); }
        inline void do_nothing() const { }
    };
    static object_creator create_object;

protected:
    ~Singleton() { }
    Singleton() = default;
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

public:
    // If, at any point (in user code), Singleton<T>::instance()
    //  is called, then the following function is instantiated.
    static T &GetInst() {
        // This is the object that we return a reference to.
        // It is guaranteed to be created before main() begins because of
        //  the next line.
        static T obj;

        // The following line does nothing else than force the instantiation
        //  of Singleton<T>::create_object, whose constructor is
        //  called before main() begins.
        create_object.do_nothing();

        return obj;
    }
};

template <typename T>
typename Singleton<T>::object_creator Singleton<T>::create_object;
