import cv2
import face_recognition
import numpy as np

class VideoCamera(object):

    def __init__(self):
        self.video = cv2.VideoCapture(0)
        self.video.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        self.video.set(cv2.CAP_PROP_FRAME_HEIGHT, 360)

        self.serik_image = face_recognition.load_image_file("serik.png")
        self.serik_face_encoding = face_recognition.face_encodings(self.serik_image)[0]

        self.altyn_image = face_recognition.load_image_file("altyn.png")
        self.altyn_face_encoding = face_recognition.face_encodings(self.altyn_image)[0]
        self.known_face_encodings = [self.serik_face_encoding, self.altyn_face_encoding]
        self.known_face_names = ["Serik", "Altyn"]

        self.face_locations = []
        self.face_encodings = []
        self.face_names = []
        self.process_this_frame = True
        
    def __del__(self):
        self.video.release()

    def get_frame(self):
        ret, frame = self.video.read()

        small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)

        rgb_small_frame = small_frame[:, :, ::-1]
        # Only process every other frame of video to save time
        if self.process_this_frame:
                # Find all the faces and face encodings in the current frame of video
                self.face_locations = face_recognition.face_locations(rgb_small_frame, number_of_times_to_upsample=2, model='hog')
                self.face_encodings = face_recognition.face_encodings(rgb_small_frame, self.face_locations, num_jitters= 3)
                
                self.face_names = []
                for face_encoding in self.face_encodings:
                    # See if the face is a match for the known face(s)
                    matches = face_recognition.compare_faces(self.known_face_encodings, face_encoding)
                    name = "Unknown"
                    
                    if True in matches:
                        first_match_index = matches.index(True)
                        name = self.known_face_names[first_match_index]
                    
                    self.face_names.append(name)

        self.process_this_frame = not self.process_this_frame
        
        
        # Display the results
        for (top, right, bottom, left), name in zip(self.face_locations, self.face_names):
            # Scale back up face locations since the frame we detected in was scaled to 1/4 size
            top *= 4
            right *= 4
            bottom *= 4
            left *= 4
            
            # Draw a box around the face
            cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)
            
            # Draw a label with a name below the face
            cv2.rectangle(frame, (left, bottom - 35), (right, bottom), (0, 0, 255), cv2.FILLED)
            font = cv2.FONT_HERSHEY_DUPLEX
            cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)
        
        ret, jpeg = cv2.imencode('.jpg', frame)
        return jpeg.tobytes()


