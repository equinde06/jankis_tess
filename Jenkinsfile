pipeline {
    agent none
    stages {
        stage('build') {
            agent { label 'pc' }
            steps {
                sh 'docker run --rm -v $WORKSPACE:/project -w /project espressif/idf:v4.4.2 idf.py build'
                stash name: "build-binaries", includes: "build/bootloader/*.bin, build/partition_table/*.bin, build/*.bin"
            }
        }

        stage('tarea-rbagente') {
            agent { label 'raspi' }
            options { skipDefaultCheckout() }
            steps {
                unstash "build-binaries"
            }
        }

        stage('load-bin-and-test-rbagent1') {
            agent { label 'raspi' }
            options { skipDefaultCheckout() }
            steps {
                script {
                    def output = sh(script: 'cd /home/easymetering/esp-serial/  && sudo ./system.sh', returnStdout: true).trim()
                    
                    if (output.contains("Error") || output.contains("failed") || output.contains("error") ) {
                        error("Output: '${output}'")
                    } else {
                        echo "Output: '${output}'"
                    }
                }
            }
            post {
                success {
                    script {
                    def output1 = sh(script: 'cd /home/easymetering/esp-serial/  && sudo ./log.sh', returnStdout: true).trim()
                    
                    if (output1.contains("Traceback") || output1.contains("[ERR]") ) {
                        error("Output: '${output1}'")
                    } else {
                        echo "Output: '${output1}'"
                    }
                }
                }
            }
        }
    }
}
