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
            cd /home/easymetering/esp-serial/  && sudo ./system.sh
            cd /home/easymetering/esp-serial/raspberry_example/build/
            sudo ./raspberry_flasher
            cd ..
            sudo rm -r build
            cd /home/easymetering/esp-serial/ binaries/app/
            sudo rm merged-flash.bin
            deactivate
            '''
        }
        post {
            success {
                sh 'echo Running tests'
            }
        }
    }
  }
}
