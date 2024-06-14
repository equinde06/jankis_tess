pipeline {
  agent none
  stages {
    stage('build') {
        agent {label 'pc'}
        steps {
          sh 'docker run --rm -v $WORKSPACE:/project -w /project espressif/idf:v4.4.2 idf.py build'
          stash name: "build-binaries", includes: "build/bootloader/*.bin, build/partition_table/*.bin, build/*.bin"
        }
    }

    stage('tarea-rbagente') {
        agent {label 'raspi'}
        options { skipDefaultCheckout() }
        steps {
            unstash "build-binaries"
            
        }
    }

    stage('load-bin-and-test-rbagent1') {
        agent {label 'raspi'}
        options { skipDefaultCheckout() }
        steps {
            sh '''
            cd /home/easymetering/esp-serial/ && sudo ./script_tess.sh
            echo "Inicio de Mergue "
            source raspi/bin/activate
            cd examples/raspberry_example/
            mkdir build && cd build
            cmake .. && cmake --build .
            cd ../../binaries/app/
            sudo chmod u+rw merged-flash.bin  
            echo "Inicio de Flash"
            cd ../../raspberry_example/build/
            sudo ./raspberry_flasher
            cd ..
            sudo rm -r build
            cd ../binaries/app/
            sudo rm merged-flash.bin
            deactivate
            ''''
        }
        post {
            success {
                sh 'echo Running tests'
            }
        }
    }
  }
}
