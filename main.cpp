#include <functional>
#include <iostream>
#include <unordered_map>

struct Event {
    virtual ~Event() {}
    virtual int getId() const = 0;
};

struct Foo : Event {
    constexpr static int eventId() {return 1;}
    int getId() const override { return eventId(); }
    std::string name() const { return "Bob"; };
};

struct Bar : Event {
    constexpr static int eventId() {return 2;}
    int getId() const override { return eventId(); };
    std::string address() const { return "42"; };
};

using EventCall = std::function<void(Event const &)> ;

template<typename E, typename S>
EventCall eventCallback(S & service) {
    return [service] (Event const & event) mutable {service.accept(static_cast<E const &>(event));};
}


struct Service {
template<typename E>
    EventCall callback() {
        return [this] (Event const & event) mutable {accept(static_cast<E const &>(event));};
    }

    void accept(Foo const & aFoo) {
        std::cout << "You gave me a Foo called " << aFoo.name() << '\n';
    }

    void accept(Bar const & aBar) {
        std::cout << "You gave me a Bar from " << aBar.address() << '\n';
    }

    void acceptFoo(Foo const & aFoo) {
        std::cout << "AcceptFoo got a Foo called " << aFoo.name() << '\n';
    }

    void callbacks(std::unordered_map<int, EventCall> & callbacks) {
        callbacks[Foo::eventId()] = [this] (Event const & event) mutable {acceptFoo(static_cast<Foo const &>(event));};
        callbacks[Bar::eventId()] = callback<Bar>();
    }

    EventCall fooCallback() {
        return [this] (Event const & event) mutable {acceptFoo(static_cast<Foo const &>(event));};
    }
};

struct Dispatcher {
    Dispatcher(std::unordered_map<int, EventCall> & callbacks) : callbacks_(callbacks) {};
    std::unordered_map<int, EventCall> & callbacks_;

    void accept(Event const & anEvent) {
        auto callback = callbacks_.find(anEvent.getId());
        if (callback != callbacks_.end()) {
            callback->second(anEvent);
        }
        else {
            std::cout << "Unknown event: " << anEvent.getId() << '\n';
        }
    }
};

int main(int, char**) {
    std::cout << "Hello, world!\n";
    Foo foo;
    Bar bar;
    std::cout << "Foo id: " << foo.getId() << " Bar id: " << bar.getId() << '\n';
    Service theService;
    theService.accept(foo);
    theService.accept(bar);
    std::unordered_map<int, EventCall> callbacks;
    theService.callbacks(callbacks);
    callbacks[Foo::eventId()](foo);
    callbacks[Bar::eventId()](bar);
    eventCallback<Foo>(theService)(foo);
    theService.fooCallback()(foo);
    Dispatcher dispatcher(callbacks);
    dispatcher.accept(bar);
    dispatcher.accept(foo);
}
