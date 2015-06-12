#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <iostream>


int main(int argc, char** argv) {
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    if(1 == argc){
        // use compiler output by default
        runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
                                                             std::cout ) );
    } else {
        // set xml outputter for jenkins
        runner.setOutputter( new CppUnit::XmlOutputter( &runner.result(),
                                                        std::cout ) );
    }
    runner.run("",
               false,
               true,
               false );
    return 0;
}