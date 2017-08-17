(begin
    (let ((ht (make-hash eq?)))
        (test (hash-count ht) 0)
        (hash-set! ht 10 20)
        (hash-set! ht 30 40)
        (test (hash-count ht) 2)
        (test (hash-exists? ht 10) #t)
        (test (hash-exists? ht 30) #t)
        (test (hash-exists? ht -90) #f)
        (test (hash-exists? ht 'aaa) #f)
        (test 20 (hash-ref ht 10))
        (test 40 (hash-ref ht 30)))

    (let ((animal-sounds (make-hash eq?)))
        (test (hash-count animal-sounds) 0)
        (hash-set! animal-sounds 'dog 'woof)
        (hash-set! animal-sounds 'cat 'meow)
        (test (hash-count animal-sounds) 2)
        (test (hash-exists? animal-sounds 'cat) #t)
        (test (hash-ref animal-sounds 'cat) 'meow)
        (hash-set! animal-sounds 'cat 'purr)
        (test (hash-exists? animal-sounds 'cat) #t)
        (test (hash-ref animal-sounds 'cat) 'purr)
        (hash-set! animal-sounds 'fox "No idea...")
        (test (hash-count animal-sounds) 3)
        (test (equal? (hash-ref animal-sounds 'fox) "No idea...") #t)
        (hash-delete! animal-sounds 'fox)
        (test (hash-count animal-sounds) 2)
        (test (hash-exists? animal-sounds 'fox) #f)
        (hash-delete! animal-sounds 'fox)
        (test (hash-count animal-sounds) 2))

    (let ((shash (make-hash equal?)))
        (hash-set! shash "name" 'John)
        (hash-set! shash "surname" 'Doe)
        (test (hash-ref shash "name") 'John)
        (test (hash-ref shash "surname") 'Doe)
        (hash-set! shash "name" 'Jane)
        (test (hash-ref shash "name") 'Jane))

    (let ((ht (make-hash eq?)))
      (define (hash-add i m) (unless (= i m) 
                               (hash-set! ht i i) (hash-add (+ i 1) m)))
      (hash-add 0 100)
      (test (hash-count ht) 100)
      (test (hash-ref ht 50) 50)
      (test (hash-exists? ht 99) #t)
      (hash-delete! ht 50)
      (test (hash-exists? ht 50) #f)
      (test (hash-count ht) 99)))


        
