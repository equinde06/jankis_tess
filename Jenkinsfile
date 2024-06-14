pipeline {
  agent none
  stages {
    stage('build') {
        agent {label 'local'}
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
               
                // Ejecutar el comando y guardar la salida en una variable
                script {
                    def output = sh(script: 'cd /home/easymetering/esp-serial/  && sudo ./system.sh', returnStdout: true).trim()
                    
                    // Verificar si la salida contiene la palabra "Error"
                    if (output.contains("Error") || output.contains("failed") ){
                        error("Se encontró un error en la salida del comando.")
                    } else {
                        echo "Sucess."
                    }
        }
        post {
            success {
                sh 'echo Running tests'
            }
        }
    }
  }
}
